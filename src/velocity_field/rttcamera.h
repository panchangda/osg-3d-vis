//
// Created by pcd on 2024/10/28.
//

#ifndef RTTCAMERA_H
#define RTTCAMERA_H
#include <osg/Node>
#include <osg/ref_ptr>
#include <osgViewer/Viewer>


namespace osg {
    class Texture2D;
}

namespace osg_3d_vis {
    class rttcamera {
    public:
        explicit rttcamera(osgViewer::Viewer& viewer);


        osg::ref_ptr<osg::Texture2D> screenColorTexture;
        osg::ref_ptr<osg::Texture2D> screenDepthTexture;
        osg::ref_ptr<osg::Texture2D> testColorTexture;
        osg::ref_ptr<osg::Node> cameraNode;
        osg::ref_ptr<osg::Geode> fullscreenQuadGeode;
        osg::ref_ptr<osg::Node> fullscreenCameraNode;
    };
}


#endif //RTTCAMERA_H
