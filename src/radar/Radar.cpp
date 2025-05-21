#include "Radar.h"

#include <qvariant.h>
#include <random>
#include <stdarg.h>
#include <osg/LineWidth>

namespace VoxelRadar{

    RadarRender* _radarrender;
    int voxels[256][256][256];
    std::unique_ptr<unsigned char[]> voxelsData = std::make_unique<unsigned char[]>(sizeof(float) * 256 * 256 * 256);
    llhRange _range;

    void initRadar(llhRange range)
    {
        for (int i = 0; i < 256; i++)
            for (int j = 0; j < 256; j++)
                for (int k = 0; k < 256; k++)
                    voxels[i][j][k] = 0;
        _range = range;
    }
    void addRadar(llhRange range)
    {
        float maxAngle = 91;
        float maxLength = 20;
        float maxSearchRange = 1;
        float minSearchRange = 1;
        float DetectionProbability = 99.9;
        _radarrender = new RadarRender();
        _radarrender->radar->MakeRGridFromFile(std::string(OSG_3D_VIS_DATA_PREFIX)+"cosec2.out");
        _radarrender->radar->SetMaxAngle_length_box(maxAngle, maxLength, maxSearchRange, minSearchRange);
        _radarrender->radar->UpdatePara();
        _radarrender->radar->UpdateDValue();
        _radarrender->SetNewPd(DetectionProbability * 0.01);

        // 将point信息放到空间体素中
        transPointsToVoxels(_radarrender->m_grid_points, _radarrender->radar->m_curd, range);
        // 将空间体素信息放到Image3D中
        for (int i = 0; i < 256; i++)
            for (int j = 0; j < 256; j++)
                for (int k = 0; k < 256; k++)
                    voxelsData[i * 256 * 256 + j * 256 + k] = voxels[j][k][i];

    }
    void submitRadar(osg::Group* grp)
    {
        osg::ref_ptr<osg::Texture3D> voxelsTexture = new osg::Texture3D;
        osg::ref_ptr<osg::Image> voxelsImage = new osg::Image;
        voxelsImage->setImage(256, 256, 256, GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, voxelsData.get(), osg::Image::USE_NEW_DELETE);
        voxelsTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        voxelsTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR); //WTF ?
        voxelsTexture->setImage(voxelsImage);

        static const std::array<uint32_t, 3> dim = { 256, 256, 256 };
        static const std::array<float, 2> lonRng = { _range.minLongtitude / M_PI * 180.f - 360.f, _range.maxLongtitude / M_PI * 180.f - 360.f };
        static const std::array<float, 2> latRng = { _range.minLatitude / M_PI * 180.f, _range.maxLatitude / M_PI * 180.f };
        static const std::array<float, 2> hRng = { _range.minHeight, _range.maxHeight };
        static const float hScale = 150.f;

        auto dvr =  std::make_shared<SciVis::ScalarViser::DirectVolumeRenderer>();
        dvr->SetDeltaT(hScale * (hRng[1] - hRng[0]) / dim[2] * .3f);
        dvr->SetMaxStepCount(600);
        dvr->AddVolume("name1", voxelsTexture, nullptr, dim, false);
        auto vol = dvr->GetVolume("name1");
        vol->SetLongtituteRange(lonRng[0], lonRng[1]);
        vol->SetLatituteRange(latRng[0], latRng[1]);
        vol->SetHeightFromCenterRange(
            static_cast<float>(osg::WGS_84_RADIUS_EQUATOR) + hScale * hRng[0],
            static_cast<float>(osg::WGS_84_RADIUS_EQUATOR) + hScale * hRng[1]);

        dvr->DisplayVolume("name1");
        grp->addChild(dvr->GetGroup());
    }
    void transPointsToVoxels(osg::ref_ptr<osg::Vec3Array> points, float curd, llhRange smallRange)
    {
        int mid = 128;
        static int localVoxels[256][256][256];

        // 求smallRange在_range中的比例，得到中心与半径
        int centerX = ((smallRange.maxLatitude + smallRange.minLatitude) / 2 - _range.minLatitude) / (_range.maxLatitude - _range.minLatitude) * 256;
        int centerY = ((smallRange.maxLongtitude + smallRange.minLongtitude) / 2 - _range.minLongtitude) / (_range.maxLongtitude - _range.minLongtitude) * 256;
        float rRatio = (smallRange.maxLatitude - smallRange.minLatitude) / (_range.maxLatitude - _range.minLatitude);

        // 清空localVoxels为false
        for (int i = 0; i < 256; i++)
            for (int j = 0; j < 256; j++)
                for (int k = 0; k < 256; k++)
                    localVoxels[i][j][k] = 0;

        for (int i = 0; i < points->size(); i++)
        {
            if (points->at(i).z() < curd)
                continue;
            int x = points->at(i).x() / 5 * rRatio;
            int y = points->at(i).y();
            if (localVoxels[centerX + x][centerY][y] == 1)
                continue;
            localVoxels[centerX + x][centerY][y] = 1;
            float stepAngle = (360.0 / 180.0) * M_PI / 3600.0;
            for (int i = 1; i < 3600; i++)
            {
                float curangle = stepAngle * i;
                float sinthita = sin(curangle);
                float costhita = cos(curangle);
                localVoxels[centerX + int(x * costhita)][centerY + int(x * sinthita)][y] = 1;
            }
        }

        for (int i = 0; i < 256; i++)
            for (int j = 0; j < 256; j++)
                for (int k = 0; k < 256; k++)
                    voxels[i][j][k] += localVoxels[i][j][k];
    }
}


// ui part
// color and drawway

static osg::ref_ptr<osg::Uniform> colorUniform = new osg::Uniform("mainColor", osg::Vec4f(1,0.6,0.6,0.4));
static osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth(2);
static osg::ref_ptr<osg::Uniform> moveSpeed = new osg::Uniform("phiThetaSpeed", osg::Vec2f(0.1, 0.05));
static osg::ref_ptr<osg::Uniform> moveTime = new osg::Uniform("t",0.0f);
static osg::ref_ptr<osg::Uniform> EmicolorUniform = new osg::Uniform("mainColor", osg::Vec4f(127 / 255, 1, 112 / 255,0.8));
enum DrawWay
{
    surface,
    line
};
static DrawWay usePattern = surface ;

class UpdateIndexCallback : public osg::NodeCallback {
public:
    UpdateIndexCallback(int begin)
        : frameCount(begin) {
        indices = new osg::DrawElementsUInt(GL_TRIANGLES);
    }
    unsigned int frameCount;
    osg::ref_ptr<osg::DrawElementsUInt> indices;

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
        osg::Geode* geode = dynamic_cast<osg::Geode*>(node);
        if (geode) {

            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(0));
            if (geometry) {
                indices->clear();
                static int loop = 0;
                osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);
                for (int i = frameCount; i <= frameCount + 30; ++i)
                {
                    indices->push_back(0);
                    indices->push_back(i % (360 + 1));
                    indices->push_back((i + 1) % (360 + 1));
                }
                geometry->setPrimitiveSet(0, indices.get());
                loop++;

                if (loop == 5) {
                    frameCount++;
                    loop = 0;
                }
            }
        }
        traverse(node, nv);
    }

};
void Radar::Radar::updateR(double value) {
    osg::Vec4 color;
    colorUniform->get(color);
    color.r() = value;
    RadarRT->getOrCreateStateSet()->getUniform("mainColor")->set(color);
};
void Radar::Radar::updateG(double value) {
    osg::Vec4 color;
    colorUniform->get(color);
    color.g() = value;
    RadarRT->getOrCreateStateSet()->getUniform("mainColor")->set(color);
};
void Radar::Radar::updateB(double value) {
    osg::Vec4 color;
    colorUniform->get(color);
    color.b() = value;
    RadarRT->getOrCreateStateSet()->getUniform("mainColor")->set(color);
};
void Radar::Radar::updateA(double value) {
    osg::Vec4 color;
    colorUniform->get(color);
    color.a() = value;
    RadarRT->getOrCreateStateSet()->getUniform("mainColor")->set(color);

};
void Radar::Radar::updateLineWidth(double value) {
    lineWidth->setWidth(value);
};
void Radar::Radar::updateDrawStyle(int index) {
    if (index == 0) {
        osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
        RadarRT->getOrCreateStateSet()->setAttribute(polyMode);
        RadarRT->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::OFF);

    }
    else if( index == 1){
        osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
        RadarRT->getOrCreateStateSet()->setAttribute(polyMode);
        RadarRT->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
    }
}
void Radar::Radar::updateEMIR(double value)
{
    osg::Vec4 color;
    EmicolorUniform->get(color);
    color.r() = value;
    Emirt->getOrCreateStateSet()->getUniform("mainColor")->set(color);
}
void Radar::Radar::updateEMIG(double value)
{
    osg::Vec4 color;
    EmicolorUniform->get(color);
    color.g() = value;
    Emirt->getOrCreateStateSet()->getUniform("mainColor")->set(color);
}
void Radar::Radar::updateEMIB(double value)
{
    osg::Vec4 color;
    EmicolorUniform->get(color);
    color.b() = value;
    Emirt->getOrCreateStateSet()->getUniform("mainColor")->set(color);
}
void Radar::Radar::updateEMIPHI(double value)
{
    osg::Vec2 speed;
    moveSpeed->get(speed);
    speed.x() = value;
    Emirt->getOrCreateStateSet()->getUniform("phiThetaSpeed")->set(speed);

}
void Radar::Radar::updateEMITHETA(double value)
{
    osg::Vec2 speed;
    moveSpeed->get(speed);
    speed.y() = value;
    Emirt->getOrCreateStateSet()->getUniform("phiThetaSpeed")->set(speed);
}

osg::ref_ptr<osg::Geometry> createCircle(osg::Vec2 center, osg::Vec2 radius, unsigned int numSegments = 360) {
    osg::ref_ptr<osg::Geometry> circleGeometry = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::LINE_LOOP, 0);
    radius *= 0.9;

    for (unsigned int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * osg::PI * float(i) / float(numSegments);
        auto la = center.x() + radius.x() * cos(angle), lo = center.y() + radius.y() * sin(angle);
        double x, y, z;
        osg_3d_vis::llh2xyz_Ellipsoid(la, lo, 360000, x, y, z);
        vertices->push_back(osg::Vec3(x, y, z));
        indices->push_back(i);
    }

    circleGeometry->setVertexArray(vertices.get());
    circleGeometry->addPrimitiveSet(indices.get());

    return circleGeometry;
}

osg::ref_ptr<osg::Geometry> MakeCircle(osg::Vec2 center, osg::Vec2 radius, unsigned int numSegments = 360) {
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES, 0);

    float angleStep = 2.0f * osg::PI / numSegments;
    double xx, yy, zz;
    osg_3d_vis::llh2xyz_Ellipsoid({ center.x(), center.y(), 360000 }, xx, yy, zz);
    vertices->push_back(osg::Vec3(xx, yy, zz));
    osg_3d_vis::llh2xyz_Ellipsoid({ center.x(), center.y(), 500 }, xx, yy, zz);
    vertices->push_back(osg::Vec3(xx, yy, zz));
    float zTop = 360000;
    float zBottom = 500;
    float zMidle = 260000;
    for (unsigned int i = 0; i < numSegments; ++i) {
        float angle = angleStep * i;
        float x = radius.x() * cos(angle);
        float y = radius.y() * sin(angle);

        osg_3d_vis::llh2xyz_Ellipsoid(center.x() + x*0.9, center.y() + y*0.9, zTop, xx, yy, zz);
        vertices->push_back(osg::Vec3(xx, yy, zz));
        float k = rand();
        k = k > 1.3 ? 1.3 : (k < 0.8 ? 0.8 : k);
        osg_3d_vis::llh2xyz_Ellipsoid(center.x() + x * k, center.y() + y * k, zMidle, xx, yy, zz);
        vertices->push_back(osg::Vec3(xx, yy, zz));

        osg_3d_vis::llh2xyz_Ellipsoid(center.x() + x, center.y() + y, zBottom, xx, yy, zz);
        vertices->push_back(osg::Vec3(xx, yy, zz));
    }
    // single index in side
    unsigned int topCenterIndex = 0;
    unsigned int midleCenterIndex = 1;
    unsigned int bottomCenterIndex = 2;
    int mod = numSegments * 3 + 2;
    // sphere
    for (unsigned int i = 2; i +3< mod; i += 3) {
        indices->push_back(topCenterIndex);
        indices->push_back(i);
        indices->push_back(i+3);
    }
    indices->push_back(topCenterIndex);
    indices->push_back(mod - 3);
    indices->push_back(2);
    for (unsigned int i = 3; i +3<mod; i += 3) {
        indices->push_back(bottomCenterIndex);
        indices->push_back(i);
        indices->push_back(i+3);
    }
    indices->push_back(bottomCenterIndex);
    indices->push_back(mod - 1);
    indices->push_back(4);

    for (unsigned int i = 2; i +5< mod; i += 3) {
        indices->push_back(i);
        indices->push_back(i + 1);
        indices->push_back(i+4);

        indices->push_back(i);
        indices->push_back(i+4);
        indices->push_back(i + 3);

        indices->push_back(i+1);
        indices->push_back(i + 2);
        indices->push_back(i+5);

        indices->push_back(i+1);
        indices->push_back(i+5);
        indices->push_back(i+4);
    }
    unsigned int lastTop = mod-3;
    unsigned int lastMid = mod - 2;
    unsigned int lastBottom = mod-1;

    indices->push_back(lastTop);
    indices->push_back(lastMid);
    indices->push_back(3);

    indices->push_back(lastTop);
    indices->push_back(2);
    indices->push_back(3);

    indices->push_back(lastMid);
    indices->push_back(lastBottom);
    indices->push_back(4);

    indices->push_back(lastMid);
    indices->push_back(3);
    indices->push_back(4);

    geometry->setVertexArray(vertices);

    geometry->addPrimitiveSet(indices.get());

    return geometry;
}

osg::ref_ptr<osg::Geometry> MakeCircleSearch(osg::Vec2 center, osg::Vec2 radius, float height= 361000, unsigned int numSegments = 360) {
    osg::ref_ptr<osg::Geometry> circleGeometry = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

    double x, y, z;
    osg_3d_vis::llh2xyz_Ellipsoid(center.x(), center.y(), height, x, y, z);
    vertices->push_back(osg::Vec3(x, y, z));
    osg::Vec3 point(x, y, z);
    radius *= 0.9;
    for (unsigned int i = 0; i < numSegments; ++i) {
        float angle = 2.0f * osg::PI * float(i) / float(numSegments);
        auto la = center.x() + radius.x() * cos(angle), lo = center.y() + radius.y() * sin(angle);

        osg_3d_vis::llh2xyz_Ellipsoid(la, lo, height, x, y, z);
        vertices->push_back(osg::Vec3(x, y, z));
    }


    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1, numSegments);
    int k = dis(gen);
    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);
    for(int i= k; i<=k+30; ++i)
    {
        indices->push_back(0);
        indices->push_back(i%(numSegments+1));
        indices->push_back((i + 1) % (numSegments + 1));
    }
    circleGeometry->setVertexArray(vertices.get());
    circleGeometry->addPrimitiveSet(indices.get());

    return circleGeometry;
}

osg::ref_ptr<osg::Geode> createCustomCylinder(const osg::Vec3& center, float radius, float height, unsigned int numSegments = 360) {
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES, 0);

    float angleStep = 2.0f * osg::PI / numSegments;
    osg::Vec3 topCenter(center.x(), center.y(), center.z() + height / 2.0f);
    osg::Vec3 bottomCenter(center.x(), center.y(), center.z() - height / 2.0f);
    double xx, yy, zz;
    osg_3d_vis::llh2xyz_Ellipsoid(topCenter, xx, yy, zz);
    vertices->push_back( osg::Vec3(xx,yy,zz));
    osg_3d_vis::llh2xyz_Ellipsoid(bottomCenter, xx, yy, zz);
    vertices->push_back(osg::Vec3(xx, yy, zz));


    for (unsigned int i = 0; i < numSegments; ++i) {
        float angle = angleStep * i;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        float zTop = topCenter.z();
        float zBottom = bottomCenter.z();
        osg_3d_vis::llh2xyz_Ellipsoid(center.x()+x, center.y()+y, zTop, xx, yy, zz);
        vertices->push_back(osg::Vec3(xx, yy, zz));
        osg_3d_vis::llh2xyz_Ellipsoid(center.x() + x*0.99, center.y() + y*0.99, zBottom, xx, yy, zz);
        vertices->push_back(osg::Vec3(xx, yy, zz));
    }

    unsigned int topCenterIndex = 0;
    unsigned int bottomCenterIndex = 1;
    int mod = numSegments * 2 + 2;
    for (unsigned int i = 2; i < numSegments * 2; i += 2) {
        unsigned int next = (i + 2) % mod;
        indices->push_back(topCenterIndex);
        indices->push_back(i);
        indices->push_back(next);
    }
    indices->push_back(topCenterIndex);
    indices->push_back(mod-2);
    indices->push_back(2);
    for (unsigned int i = 3; i < numSegments * 2; i += 2) {
        unsigned int next = (i + 2) % (numSegments * 2 + 2);
        indices->push_back(bottomCenterIndex);
        indices->push_back(i );
        indices->push_back(next);
    }
    indices->push_back(bottomCenterIndex);
    indices->push_back(mod-1);
    indices->push_back(3);

    for (unsigned int i = 2; i < numSegments * 2; i += 2) {
        unsigned int next = (i + 2) % (numSegments * 2);
        indices->push_back(i);
        indices->push_back(i + 1);
        indices->push_back(next + 1);

        indices->push_back(i);
        indices->push_back(next + 1);
        indices->push_back(next);
    }
    unsigned int lastTop = numSegments * 2 ;
    unsigned int lastBottom = numSegments * 2 + 1;
    indices->push_back(lastTop);
    indices->push_back(lastBottom);
    indices->push_back(3);

    indices->push_back(lastBottom);
    indices->push_back(2);
    indices->push_back(3);
    geometry->setVertexArray(vertices);

    geometry->addPrimitiveSet(indices.get());

    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    geode->addDrawable(geometry.get());

    return geode;
}
Radar::Radar::Radar(osgViewer::Viewer& viewer, osg::ref_ptr<osg::Group> root)
{
    setCamera(viewer.getCamera());


    Addllh(osg_3d_vis::llhRange(25.l, 35., 105., 115.));
    Addllh(osg_3d_vis::llhRange(14., 19., 90., 100.));

    Addllh(osg_3d_vis::llhRange(37., 39., 100., 102.));
    Addllh(osg_3d_vis::llhRange(30., 39., 130., 140.));
    Addllh(osg_3d_vis::llhRange(14., 20., 95., 100.));

    Addllh(osg_3d_vis::llhRange(10., 20., 110., 120.));
    Addllh(osg_3d_vis::llhRange(5., 10., 110., 120.));
    GenerateRadarMesh();
    GenerateRadarlineMesh();
    GenerateRadarsearchMesh();
    root->addChild(RadarRT);
    root->addChild(Radarline);
    root->addChild(Radarsearch);

    addEmi({ 16,115,5 });
    GenerateEmiMesh();
    root->addChild(Emirt);
}

void Radar::Radar::GenerateRadarMesh()
{

    RadarRT = new osg::Group;
    for (int i = 0; i < Circle.size(); ++i) {
        RadarRT->addChild(Circle[i]);
    }

    osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
    osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
    VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanVS.glsl");
    FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanPS.glsl");
    osg::ref_ptr<osg::Program> Program = new osg::Program;
    Program->addShader(VertexShader);
    Program->addShader(FragmentShader);
    osg::StateSet * StateSet = RadarRT->getOrCreateStateSet();

    StateSet->setAttributeAndModes(Program);
    StateSet->addUniform(colorUniform);
    StateSet->addUniform(mvpUniform);

    StateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    StateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

}

void Radar::Radar::GenerateRadarlineMesh()
{
    Radarline = new osg::Group;
    for (int i = 0; i < CircleLine.size(); ++i) {
        Radarline->addChild(CircleLine[i]);
    }

    osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
    osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
    VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanVS.glsl");
    FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanPS.glsl");
    osg::ref_ptr<osg::Program> Program = new osg::Program;
    Program->addShader(VertexShader);
    Program->addShader(FragmentShader);
    osg::StateSet * StateSet = Radarline->getOrCreateStateSet();

    StateSet->setAttributeAndModes(Program);
    StateSet->addUniform(new osg::Uniform("mainColor", osg::Vec4(1,1,1,1)));
    StateSet->addUniform(mvpUniform);

    StateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    StateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
    StateSet->setAttribute(polyMode, osg::StateAttribute::OVERRIDE);
    StateSet->setAttributeAndModes(new osg::LineWidth(0.8), osg::StateAttribute::ON);
}

void Radar::Radar::GenerateRadarsearchMesh()
{
    Radarsearch = new osg::Group;
    for (int i = 0; i < Circlesearch.size(); ++i) {
        Radarsearch->addChild(Circlesearch[i]);
    }

    osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
    osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
    VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanVS.glsl");
    FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanPS.glsl");
    osg::ref_ptr<osg::Program> Program = new osg::Program;
    Program->addShader(VertexShader);
    Program->addShader(FragmentShader);
    osg::StateSet * StateSet = Radarsearch->getOrCreateStateSet();

    StateSet->setAttributeAndModes(Program);
    StateSet->addUniform(new osg::Uniform("mainColor", osg::Vec4(0.5, 1, 0, 0.6)));
    StateSet->addUniform(mvpUniform);

    StateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    StateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

}

void Radar::Radar::GenerateEmiMesh()
{
    Emirt = new osg::Group;
    for (int i = 0; i < EmiGeos.size(); ++i) {
        Emirt->addChild(EmiGeos[i]);
    }

    osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
    osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
    VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/EmiVS.glsl");
    FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/EmiPS.glsl");
    osg::ref_ptr<osg::Program> Program = new osg::Program;
    Program->addShader(VertexShader);
    Program->addShader(FragmentShader);
    osg::StateSet * StateSet = Emirt->getOrCreateStateSet();

    StateSet->setAttributeAndModes(Program);
    StateSet->addUniform(EmicolorUniform);
    StateSet->addUniform(mvpUniform);
    StateSet->addUniform(moveSpeed);
    StateSet->addUniform(moveTime);
    moveTime->setUpdateCallback(new osg_3d_vis::TimeUniformCallback());
    StateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    StateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    StateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

osg::ref_ptr<osg::Group> Radar::Radar::GenerateEmi(osg_3d_vis::llhRange range)
{
    osg::ref_ptr<osg::Group> geodeGroup = new osg::Group;

    auto [minLa, maxLa, minLo, maxLo, minH, maxH] = range;
    osg::Vec2 center = osg::Vec2(minLa , minLo );
    osg::Vec2 radiusMAX = osg::Vec2(maxLa - minLa, maxLo - minLo) ;

    geodeGroup->addChild(createCustomCylinder(osg::Vec3(center, (minH + maxH)/2), radiusMAX.x(), (maxH-minH)));
    geodeGroup->addChild(createCustomCylinder(osg::Vec3(center, maxH+50000), radiusMAX.x()/10, 100000));

    return geodeGroup;

}



osg::ref_ptr<osg::Geode>  Radar::Radar::Generate(osg_3d_vis::llhRange range)
{
    osg::ref_ptr<osg::Geode> Geode = new osg::Geode;
    auto [minLa, maxLa, minLo, maxLo, minH, maxH] = range;
    osg::Vec2 center = osg::Vec2(minLa + maxLa, minLo + maxLo) / 2;
    osg::Vec2 radiusMAX = osg::Vec2(maxLa - minLa, maxLo - minLo) / 2;


    Geode->addChild(MakeCircle(center, radiusMAX));

    return Geode;
}

osg::ref_ptr<osg::Group> Radar::Radar::GenerateCiecleline(osg_3d_vis::llhRange range)
{
    osg::ref_ptr<osg::Group> geodeGroup = new osg::Group;
    auto [minLa, maxLa, minLo, maxLo, minH, maxH] = range;
    osg::Vec2 center = osg::Vec2(minLa + maxLa, minLo + maxLo) / 2;
    osg::Vec2 radiusMAX = osg::Vec2(maxLa - minLa, maxLo - minLo) / 2;
    const int level = 5;


    // circle line part
    for (int i = 1; i <= level; ++i)
    {
        auto r = radiusMAX * (float)i / level;
        osg::ref_ptr<osg::Geode> geode = new osg::Geode;
        geode->addDrawable(createCircle(center, r));
        geodeGroup->addChild(geode.get());
    }

    return geodeGroup;
}

osg::ref_ptr<osg::Geode> Radar::Radar::GenerateCieclesearch(osg_3d_vis::llhRange range)
{
    osg::ref_ptr<osg::Geode> Geode = new osg::Geode;
    auto [minLa, maxLa, minLo, maxLo, minH, maxH] = range;
    osg::Vec2 center = osg::Vec2(minLa + maxLa, minLo + maxLo) / 2;
    osg::Vec2 radiusMAX = osg::Vec2(maxLa - minLa, maxLo - minLo) / 2;

    Geode->addDrawable(MakeCircleSearch(center, radiusMAX));
    auto list = dynamic_cast<osg::DrawElementsUInt*>(dynamic_cast<osg::Geometry*>(Geode->getDrawable(0))->getPrimitiveSet(0))->at(1);
    Geode->addUpdateCallback(new UpdateIndexCallback(list));
    return Geode;
}
