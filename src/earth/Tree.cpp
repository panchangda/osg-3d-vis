//
// Created by pcd on 2024/11/6.
//

//
// Created by pcd on 2024/9/29.
//

#include "Tree.h"

#include <osg/BlendFunc>
#include <osg/PrimitiveSet>

#include <osgDB/ReadFile>
#include "../globals.h"

#include "../util.h"
#include "osg/MatrixTransform"
#include "osg/PositionAttitudeTransform"

const std::string treeOBJ = std::string(OSG_3D_VIS_DATA_PREFIX) + "gpu-instances/trees9.obj";
const std::string vs = std::string(OSG_3D_VIS_SHADER_PREFIX) + "gpu-instances/tree.vert";
const std::string fs = std::string(OSG_3D_VIS_SHADER_PREFIX) + "gpu-instances/tree.frag";

const osg::Vec4 lightDirections[] = {
    osg::Vec4(-1,  0,  0, 0),
    osg::Vec4(1,   0,  0, 0),
    osg::Vec4(0,   0, -1, 0),
    osg::Vec4(0,   0,  1, 0),
    osg::Vec4(0,   1,  0, 0),
    osg::Vec4(0,  -1,  0, 0),
    osg::Vec4(-1,  0,  1, 0),
    osg::Vec4(1,   0, -1, 0),
    osg::Vec4(0,   1, -1, 0),
    osg::Vec4(0,  -1,  1, 0),
    osg::Vec4(0.433737, -0.751254, 0.497484, 0),
    osg::Vec4(-0.433737, 0.751254, -0.497484, 0),
};

osg::ref_ptr<osg::LightSource> createLightSource(int lightNum, const osg::Vec4& direction) {
    osg::ref_ptr<osg::Light> light = new osg::Light();
    light->setLightNum(lightNum); // GL_LIGHT0, GL_LIGHT1, ...
    light->setPosition(direction); // 设置方向光方向（w = 0 表示平行光）
    light->setDiffuse(osg::Vec4(1.0, 1.0, 1.0, 1.0));  // 漫反射颜色
    light->setSpecular(osg::Vec4(1.0, 1.0, 1.0, 1.0)); // 镜面反射颜色
    light->setAmbient(osg::Vec4(0.2, 0.2, 0.2, 1.0));  // 环境光颜色

    osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
    lightSource->setLight(light);
    return lightSource;
}

void addLightsToScene(osg::ref_ptr<osg::Group> root) {
    int maxLights = sizeof(lightDirections) / sizeof(lightDirections[0]);
    for (int i = 0; i < maxLights; ++i) {
        osg::ref_ptr<osg::LightSource> lightSource = createLightSource(i, lightDirections[i]);
        root->addChild(lightSource); // 将光源添加到场景图
    }
}

void configureSwitch(osg::ref_ptr<osg::Group> root) {
    if (!root) return;

    osg::ref_ptr<osg::Switch> switchNode = new osg::Switch();

    // 将所有子节点添加到 Switch 中
    for (unsigned int i = 0; i < root->getNumChildren(); ++i) {
        switchNode->addChild(root->getChild(i), false); // 初始全部隐藏
    }

    // 启用指定子节点
    switchNode->setValue(0, true); // 渲染第 0 个子模型

    // 替换原始 Group
    root->removeChildren(0, root->getNumChildren());
    root->addChild(switchNode);
}
namespace osg_3d_vis {


    Tree::Tree(osg::ref_ptr<osg::Group> _root, osg::ref_ptr<osg::Camera> _camera)
        :root(_root),viewerCamera(_camera)
    {
        Geos = new osg::Geode;
        osg::ref_ptr<osg::Node> objModel = osgDB::readNodeFile(treeOBJ.c_str());



        if (!objModel) {
            std::cerr << "Failed to load model.obj" << std::endl;
            exit(1);
        }
        auto gp = objModel->asGroup();
        if( gp == NULL ) {
            std::cerr << "Failed to load model.obj" << std::endl;
            exit(1);
        }
        configureSwitch(gp);
        /*GeometryExtractor geoExtractor;
        objModel->accept(geoExtractor);

        for(int i=0;i<1; ++i) {
            CreareSingleGeode(geoExtractor,i);
        }


        setShader();
        setUniforms();*/
        /*
        objModel->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
        */
        /*
        addLightsToScene(root);
        */

        float step = 0.4;
        int p = 5;

        auto matsUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "offset", 100);
        std::vector<osg::Matrixf> matrices;
        for (int i = -5; i < 5; ++i)
        {
            for (int j = -5; j < 5; ++j)
            {

                auto pos = llh2xyz_Ellipsoid(osg::DegreesToRadians(30 + step * i), osg::DegreesToRadians(300 + step * j), 20000);

                osg::Matrix rotM = osg::Matrix::rotate(osg::Z_AXIS,osg::Vec3f(pos));
                osg::Matrix transM = osg::Matrix::translate(pos);
                osg::Matrix scaleM = osg::Matrix::scale(osg::Vec3(8000, 8000, 8000));

                auto mat = osg::Matrix( scaleM * rotM * transM );
                osg::MatrixTransform* transform = new osg::MatrixTransform(mat);
                transform->addChild(gp);
                root->addChild(transform);
            }
        }
     }

    void Tree::setShader() {
        osg::ref_ptr<osg::StateSet> stateset = Geos->getOrCreateStateSet();

        // 创建着色器程序
        osg::ref_ptr<osg::Program> shaderProgram = new osg::Program;

        osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
        osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);

        vertexShader->loadShaderSourceFromFile(vs);
        fragmentShader->loadShaderSourceFromFile(fs);

        shaderProgram->addShader(vertexShader.get());
        shaderProgram->addShader(fragmentShader.get());

        shaderProgram->addBindAttribLocation("Position", 0);
        shaderProgram->addBindAttribLocation("TexCoord", 1);


        Geos->getOrCreateStateSet()->setAttributeAndModes(shaderProgram, osg::StateAttribute::ON);

    }

    void Tree::setUniforms() {
        osg::ref_ptr<osg::StateSet> stateset = Geos->getOrCreateStateSet();

        // views
        auto modelUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "model");
        modelUniform->setUpdateCallback(new osg_3d_vis::ModelMatrixCallback(viewerCamera));
        auto viewUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "view");
        viewUniform->setUpdateCallback(new osg_3d_vis::ViewMatrixCallback(viewerCamera));
        auto projUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "proj");
        projUniform->setUpdateCallback(new osg_3d_vis::ProjectionMatrixCallback(viewerCamera));

        stateset->addUniform(modelUniform);
        stateset->addUniform(viewUniform);
        stateset->addUniform(projUniform);

        // time & wind
        auto timerUniform = new osg::Uniform(osg::Uniform::FLOAT, "time");
        timerUniform->setUpdateCallback(new osg_3d_vis::TimerCallback());
        stateset->addUniform(timerUniform);
        stateset->addUniform(new osg::Uniform("windDirection", osg::Vec3(1.0f, 0.0f, 1.0f)));


        /*float step = 3;
        int p = 5;

        auto matsUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "offset", 100);
        std::vector<osg::Matrixf> matrices;
        for (int i = -5; i < 5; ++i)
        {
            for (int j = -5; j < 5; ++j)
            {

                double x, y, z;
                auto llh = osg::Vec3{ 30 + step * i ,300 + step * j ,200000 };
                llh2xyz_Ellipsoid(osg::Vec3d(osg::DegreesToRadians(30 + step * i), osg::DegreesToRadians(300 + step * j), 200000), x, y, z);
                auto pos = osg::Vec3(x, y, z);

                osg::Matrix rotM = osg::Matrix::rotate(osg::Z_AXIS,pos);
                osg::Matrix transM = osg::Matrix::translate(pos);
                osg::Matrix scaleM = osg::Matrix::scale(osg::Vec3(20000, 20000, 20000));

                auto mat = osg::Matrix( scaleM * rotM * transM );
                matrices.push_back(mat);
            }
        }
		for(int i=0; i<100; ++i)
		{
            matsUniform->setElement(i, matrices[i]);
		}
        stateset->addUniform(matsUniform);*/
    }


    void Tree::CreareSingleGeode(const GeometryExtractor &data, int index) {
        osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
        geom->setVertexAttribArray(0, data.geometries[index].vertices, osg::Array::BIND_PER_VERTEX);
        geom->setVertexAttribArray(1, data.geometries[index].uvs, osg::Array::BIND_PER_VERTEX);
        geom->setVertexAttribArray(2, data.geometries[index].noramls, osg::Array::BIND_PER_VERTEX);
        auto Primitives = data.geometries[index].indices;
        // key of instance draw 
    	geom->setUseDisplayList(false);
        Primitives->setNumInstances(instanceCount);
        geom->addPrimitiveSet(Primitives);


        // samplers
        osg::ref_ptr<osg::StateSet> stateset = geom->getOrCreateStateSet();
        stateset->setTextureAttributeAndModes(0, data.geometries[index].textures[0], osg::StateAttribute::ON);
        Geos->addDrawable(geom);
    }

}
