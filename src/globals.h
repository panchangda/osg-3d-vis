//
// Created by pcd on 2024/9/16.
//

#ifndef GLOBALS_H
#define GLOBALS_H

// control earth showing & effects scales
namespace osg_3d_vis{
    // Earth
    constexpr bool drawEarth = true;


    // Particles:
    constexpr bool drawWeather = true;
    constexpr bool drawExplode = false;
    constexpr bool enableFog = false;
    constexpr bool fogLinear = false;


    // 3D Charts:
    constexpr bool showCharts = false;

    // Frame Timer
    extern float frameTime;
    extern float frameFPS;

    // Camera
    constexpr bool setHomePos = true;
    extern osg::Vec3d cameraPosition;
    extern osg::Vec3d cameraCenter;
}


#endif //GLOBALS_H
