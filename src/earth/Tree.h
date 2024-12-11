//
// Created by pcd on 2024/11/6.
//

#ifndef TREE_H
#define TREE_H

#include <osg/ref_ptr>
#include <osg/Group>
#include <osg/NodeVisitor>     // For osg::NodeVisitor
#include <osg/Geode>           // For osg::Geode
#include <osg/Geometry>        // For osg::Geometry and related types
#include <osg/Texture>         // For osg::Texture
#include <osg/StateSet>        // For osg::StateSet

#include <iostream>
#include <qnamespace.h>

namespace osg_3d_vis {
	class GeometryExtractor;


	class Tree {
    public:
        explicit Tree(osg::ref_ptr<osg::Group> _root, osg::ref_ptr<osg::Camera> _camera);
        void createInstancePos() {
            instancePos = std::vector<osg::Vec3>(instanceCount, osg::Vec3d());
            int X = std::pow(instanceCount, 0.5);
            int Y = std::pow(instanceCount, 0.5);

            for(int i=0;i<X;i++) {
                for(int j=0;j<Y;j++) {
                    instancePos[i*X + j] = osg::Vec3( 0,(i+j)*20,0);
                }
            }
        }
        void setShader();
        void setUniforms();
        void CreareSingleGeode(const GeometryExtractor& data, int index);
      private:
        osg::ref_ptr<osg::Geode> Geos;
        osg::ref_ptr<osg::Camera> viewerCamera;
        osg::ref_ptr<osg::Group> root;
        osg::ref_ptr<osg::Geode> quads;
        const int instanceCount = 100;
        std::vector<osg::Vec3> instancePos;
    };


}


#endif //TREE_H
