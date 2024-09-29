//
// Created by pcd on 2024/9/16.
//

#ifndef EARTH_HPP
#define EARTH_HPP
#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <osg/Point>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <osgUtil/Tessellator>
#include "globals.h"

// use high definition earth image
// #define USE_HD_EARTH_IMAGE

namespace osg_3d_vis {


    class Earth {
    public:

        Earth(osg::ref_ptr<osg::Group> _root): root(_root) {
            root->addChild(createEarth());
        }

        static inline osg::ref_ptr<osg::CoordinateSystemNode> createEarth() {
            auto* hints = new osg::TessellationHints;
            hints->setDetailRatio(5.0f);

            auto* sd = new osg::ShapeDrawable(
                new osg::Sphere(osg::Vec3(0.0, 0.0, 0.0), osg::WGS_84_RADIUS_POLAR), hints);
            sd->setUseVertexBufferObjects(true);
            //sd->setUseVertexArrayObject(true);

            auto* geode = new osg::Geode;
            geode->addDrawable(sd);
            auto filename = std::string(OSG_3D_VIS_DATA_PREFIX) +
        #ifdef USE_HD_EARTH_IMAGE
                "Land_shallow_topo_2048.jpg";
#else
                    "1024px-Land_shallow_topo_2048.jpg";
#endif

            geode->getOrCreateStateSet()->setTextureAttributeAndModes(
                0, new osg::Texture2D(osgDB::readImageFile(filename)));
            ;
            osg::ref_ptr<osg::CoordinateSystemNode> csn = new osg::CoordinateSystemNode;
            osg::EllipsoidModel* epmodel = new osg::EllipsoidModel();
            csn->setEllipsoidModel(epmodel);
            csn->addChild(geode);

            return csn;

        }

    private:
        osg::ref_ptr<osg::Group> root;
    };
}





#endif //EARTH_HPP
