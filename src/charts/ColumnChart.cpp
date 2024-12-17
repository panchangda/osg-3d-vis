#include "ColumnChart.h"

#include "osg/CoordinateSystemNode"
#include "osg/ShapeDrawable"
#include <osg/Material>
#include <array>
#include "../Util.h"
#include <random>
using namespace std;

osg::ref_ptr<osg::Geometry> ColumnChart::drawQuads(osg::Vec3 location, osg::Vec3 color, float height = 0)
{
    auto vec3 = new osg::Vec3Array;
    auto llh = osg_3d_vis::llhRange(location.x() - step, location.x() + step, 
        location.y() - step, location.y() + step, baseHeight, baseHeight + location.z());

    auto [minLa, maxLa, minLo, maxLo, minH, maxH] = llh;
    double x, y, z;
    osg_3d_vis::llh2xyz_Ellipsoid(minLa, minLo, minH, x, y, z);
    vec3->push_back(osg::Vec3( x,y,z ));
    osg_3d_vis::llh2xyz_Ellipsoid(maxLa, minLo, minH, x, y, z);
    vec3->push_back(osg::Vec3(x, y, z));
    osg_3d_vis::llh2xyz_Ellipsoid(maxLa, maxLo, minH, x, y, z);
    vec3->push_back(osg::Vec3(x, y, z));
    osg_3d_vis::llh2xyz_Ellipsoid(minLa, maxLo, minH, x, y, z);
    vec3->push_back(osg::Vec3(x, y, z));
    osg_3d_vis::llh2xyz_Ellipsoid(minLa, minLo, maxH, x, y, z);
    vec3->push_back(osg::Vec3(x, y, z));
    osg_3d_vis::llh2xyz_Ellipsoid(maxLa, minLo, maxH, x, y, z);
    vec3->push_back(osg::Vec3(x, y, z));
    osg_3d_vis::llh2xyz_Ellipsoid(maxLa, maxLo, maxH, x, y, z);
    vec3->push_back(osg::Vec3(x, y, z));
    osg_3d_vis::llh2xyz_Ellipsoid(minLa, maxLo, maxH, x, y, z);
    vec3->push_back(osg::Vec3(x, y, z));

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();



    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES, 36);


    unsigned int idx[] = {
        0, 1, 2,  0, 2, 3,    // 前面
        4, 5, 6,  4, 6, 7,    // 后面
        0, 1, 5,  0, 5, 4,    // 底面
        2, 3, 7,  2, 7, 6,    // 顶面
        0, 3, 7,  0, 7, 4,    // 左面
        1, 2, 6,  1, 6, 5     // 右面
    };

    for (int i = 0; i < 36; ++i) {
        indices->at(i) = idx[i];
    }

    geometry->addPrimitiveSet(indices.get());
    geometry->setVertexAttribArray(0, vec3, osg::Array::BIND_PER_VERTEX);
    geometry->getOrCreateStateSet()->addUniform(new osg::Uniform("mainColor", osg::Vec4(color,0.4)));
    return geometry;

}


osg::ref_ptr<osg::Geode> ColumnChart::generateColumn(osg::ref_ptr<osgViewer::Viewer> viwer)
{

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0.f, 2000000.f);


    // 创建 osg::Vec3Array
    osg::ref_ptr<osg::Vec3Array> vecArray = new osg::Vec3Array;


    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    const std::array<osg::Vec3, 8> colors{
        osg::Vec3(1.0f, 0.0f, 0.0f),  // 红色
        osg::Vec3(0.0f, 1.0f, 0.0f),  // 绿色
        osg::Vec3(0.0f, 0.0f, 1.0f),  // 蓝色
        osg::Vec3(1.0f, 1.0f, 0.0f),  // 黄色
        osg::Vec3(0.0f, 1.0f, 1.0f),  // 青色
        osg::Vec3(1.0f, 0.0f, 1.0f),  // 品红
        osg::Vec3(1.0f, 1.0f, 1.0f),  // 白色
        osg::Vec3(0.0f, 0.0f, 0.0f)   // 黑色
    };
    for (int i = -179; i < 180; i += 2) {
        for (int j = 1; j < 360; j += 2)
        {
            auto v = osg::Vec3{ (float)i, (float)j, (float)dis(gen)};
            if (i % 5 == 0 && j % 5 == 0) v.z() ;
            else v.z() /= 10;
            cout << v.x() << ' ' << v.y() << ' ' << v.z() << endl;
            geode->addChild(drawQuads( v,colors[ abs(i)/46%8]));
        }
    }

    auto state = geode->getOrCreateStateSet();
    osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
    osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
    VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "charts/vs.glsl");
    FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "charts/ps.glsl");
    osg::ref_ptr<osg::Program> Program = new osg::Program;
    Program->addShader(VertexShader);
    Program->addShader(FragmentShader);
    state->setAttributeAndModes(Program);
    osg::Uniform* mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "mvp");
    mvpUniform->setUpdateCallback(new osg_3d_vis::ModelViewProjectionMatrixCallback(viwer->getCamera()));
    state->addUniform(mvpUniform);
    state->setMode(GL_BLEND, osg::StateAttribute::ON);
    state->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    state->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
    return geode;
}
