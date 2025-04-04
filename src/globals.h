//
// Created by pcd on 2024/9/16.
//

#ifndef GLOBALS_H
#define GLOBALS_H
#include <map>
#include <osg/Vec3d>
#include <osg/View>

#include "util.h"
#include "velocity_field/StreamLineMainWindow.h"

// control earth showing & effects scales
namespace osg_3d_vis{

    // debug setups
    constexpr osg::NotifySeverity osgNotifyLevel = osg::NOTICE;

    // Earth
    constexpr bool drawEarth = true;
    constexpr float earthScale = 1000.0f;
    constexpr bool drawGrass = false;
    constexpr  bool drawTree = false;
    constexpr bool showCloud = false;
    // Particles:
    constexpr bool drawCessna = false;
    constexpr bool drawWeather = false;
    constexpr bool drawExplode = false;
    constexpr bool enableFog = false;
    constexpr bool fogLinear = false;

    constexpr bool pbr = false;

    // 3D Charts:
    constexpr bool showCharts = false;


    // Frame Timer
    extern float frameTime;
    extern float frameFPS;

    // Camera
    struct CameraViewParams {
        osg::Vec3d position;
        osg::Vec3d center;
        osg::Vec3d up;
    };

    enum class ECity{
        HangZhou,
        BeiJing,
        MAX
    };


    constexpr bool bSetCameraToCity = false;
    constexpr ECity cameraTargetCity = ECity::HangZhou;
    extern osg::Vec3d cameraPosition;
    extern osg::Vec3d cameraCenter;
    constexpr osg::View::LightingMode lightingMode = osg::View::NO_LIGHT;


    const osg::Vec3d instanceLLH{ osg::DegreesToRadians(30.0f),
        osg::DegreesToRadians(120.0f + 180.0f),
        10000};

    const osg::Vec3d HangZhouPos = llh2xyz_Ellipsoid(osg::DegreesToRadians(30.0f),
                osg::DegreesToRadians(120.0f+180.0f),
                10000.0f);
    const std::map<ECity, CameraViewParams> cameraCityMap = {
        {
            ECity::HangZhou,
            CameraViewParams{
                osg::Vec3d(HangZhouPos.x()+100000.0f, HangZhouPos.y()+100000.0f, HangZhouPos.z()+100000.0f),
                HangZhouPos,
                osg::Vec3d(0.0f, -1.0f, 0.0f)
            }
        }
    };

}


#endif //GLOBALS_H
