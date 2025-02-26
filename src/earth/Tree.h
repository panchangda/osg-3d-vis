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

#include <qnamespace.h>

namespace osg_3d_vis {
	class GeometryExtractor;


	class Tree {
    public:
        explicit Tree(osg::ref_ptr<osg::Group> _root, osg::ref_ptr<osg::Camera> _camera);
        void setShader();
        void setUniforms();
        void CreareSingleGeode(const GeometryExtractor& data, int index);
      private:
        osg::ref_ptr<osg::Geode> Geos;
        osg::ref_ptr<osg::Camera> viewerCamera;
        osg::ref_ptr<osg::Group> root;
        osg::ref_ptr<osg::Geode> quads;
        const int instanceCount = 100;
        std::vector<osg::Matrix> instancePos;
    };


}


#endif //TREE_H
