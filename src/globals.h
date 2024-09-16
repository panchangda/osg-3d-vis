//
// Created by pcd on 2024/9/16.
//

#ifndef GLOBALS_H
#define GLOBALS_H

// control earth showing & effects scales
namespace osg_3d_vis{
    constexpr bool drawEarth = true;

    // Particles:
    constexpr bool drawWeather = false;
    constexpr bool drawExplode = false;
    constexpr bool enableFog = false;
    constexpr bool fogLinear = false;


    // 3D Charts:
    constexpr bool showCharts = false;

    // Frame Timer
    extern float frameTime;
    extern float frameFPS;
}


#endif //GLOBALS_H
