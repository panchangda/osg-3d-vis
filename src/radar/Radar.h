#pragma once
#include <osg/ShapeDrawable>
#include <osg/Texture3D>

#include <osg/PrimitiveSet>
#include<osg/PolygonMode>
#include <osg/Texture2D>
#include <osgViewer/Viewer>
#include <osg/Group>


#include "../util.h"
#include "APMRadarRender.h"
#include <iostream>
#include <osgDB/WriteFile>
#include <array>
#include "../volumeRender.h"
#include <memory>

using namespace  std;

namespace VoxelRadar{
    using osg_3d_vis::llhRange;

    extern RadarRender* _radarrender;
    extern int voxels[256][256][256];
    extern std::unique_ptr<unsigned char[]> voxelsData;
    extern llhRange _range;

    void initRadar(llhRange range = llhRange());
    void addRadar(llhRange range = llhRange());
    void submitRadar(osg::Group* grp);

    void transPointsToVoxels(osg::ref_ptr<osg::Vec3Array> points, float curd, llhRange smallRange);
}


namespace Radar {
    class Radar {
    public:
        Radar(osgViewer::Viewer& viewer, osg::ref_ptr<osg::Group> root);
        osg::Uniform* mvpUniform;
        osg::ref_ptr<osg::Camera> camera;
        void setCamera(osg::Camera* cam) {
            camera = cam;
            mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "mvp");
            mvpUniform->setUpdateCallback(new osg_3d_vis::ModelViewProjectionMatrixCallback(camera));
        };


        std::vector<osg::ref_ptr<osg::Geode>> Circle;
        std::vector<osg::ref_ptr<osg::Group>> CircleLine;
        std::vector<osg::ref_ptr<osg::Group>> Circlesearch;
        osg::ref_ptr<osg::Group> RadarRT;
        osg::ref_ptr<osg::Group> Radarline;
        osg::ref_ptr<osg::Group> Radarsearch;
        void GenerateRadarMesh();
        void GenerateRadarlineMesh();
        void GenerateRadarsearchMesh();


        // for EMI draw
        std::vector<osg::ref_ptr<osg::Group>> EmiGeos;
        osg::ref_ptr<osg::Group> Emirt;


        void GenerateEmiMesh();

        void Addllh(osg_3d_vis::llhRange range)
        {
            Circle.push_back(Generate(range));
            CircleLine.push_back(GenerateCiecleline(range));
            Circlesearch.push_back(GenerateCieclesearch(range));
        }

        void addEmi(osg::Vec3 fac)
        {

            osg_3d_vis::llhRange range{ fac.x(), fac.x() + fac.z(), fac.y(),fac.y() + fac.z(), 360000,360000 +200000 };
            EmiGeos.push_back(GenerateEmi(range));
        }
        osg::ref_ptr<osg::Geode>  Generate(osg_3d_vis::llhRange range);
        osg::ref_ptr<osg::Group>  GenerateCiecleline(osg_3d_vis::llhRange range);
        osg::ref_ptr<osg::Geode>  GenerateCieclesearch(osg_3d_vis::llhRange range);
        osg::ref_ptr<osg::Group>  GenerateEmi(osg_3d_vis::llhRange range);

        //for ui part
        void updateR(double value);
        void updateG(double value);
        void updateB(double value);
        void updateA(double value);
        void updateLineWidth(double value);
        void updateDrawStyle(int index);

        void updateEMIR(double value);
        void updateEMIG(double value);
        void updateEMIB(double value);
        void updateEMIPHI(double value);
        void updateEMITHETA(double value);
    };

};
