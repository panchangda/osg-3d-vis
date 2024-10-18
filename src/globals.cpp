#include "globals.h"

// osg_3d_vis non const vars should only be initialized here && once
namespace osg_3d_vis{


    float frameTime;
    float frameFPS;

    // Camera
    osg::Vec3d cameraPosition = osg::Vec3d(0,0,0);
    osg::Vec3d cameraCenter = osg::Vec3d(0,0,0);
}

