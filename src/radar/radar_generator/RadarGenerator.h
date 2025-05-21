#pragma once

#include <osgViewer/Viewer>
#include <osgViewer/View>
#include <osg/GLExtensions>
#include <osg/GraphicsContext>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Node>
#include <osg/Material>
#include <osg/StateSet>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Program>
#include <osg/Shader>
#include <osgDB/ReadFile>
#include <osg/Texture2D>
#include <osg/Texture3D>
#include <osg/TexGen>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/AnimationPath>
#include <osg/LineWidth>
#include <algorithm>

#include "../../util.h"

namespace Radar
{
    using namespace osg_3d_vis;

    class RadarGenerator
    {
    public:
        enum class Type
        {
            Hemisphere,
            ConcaveHemisphere,
        };
        struct Settings
        {
            float radius = 1.0;
            int slices = 24;
            int stacks = 12;
        };
        enum class PolygonMode
        {
            Fill,
            Line
        };
        struct VisualParameters
        {
            PolygonMode polygonMode = PolygonMode::Line;
            float LineWidth = 1.0f;
            bool bAnimated = true;
        };

        RadarGenerator(Type type = Type::Hemisphere, Settings settings = {1.0, 24, 12});

        osg::ref_ptr<osg::Group> addToGroup(osg::Group *root, osg::Camera *camera, llhRange range, VisualParameters parameter = {PolygonMode::Line, 1.0, true}) const;

        static void setEMI(osg::Group *baseRadar, llhRange range);

    private:
        float radius = 1.0f;
        int slices = 24;
        int stacks = 12;
        Type type;

        std::vector<osg::ref_ptr<osg::Geometry>> coverageGeometries;
        std::vector<osg::ref_ptr<osg::Geometry>> scanningGeometries;
        std::vector<osg::ref_ptr<osg::Geometry>> linesGeometries;

        void setGeometriesByType(Type type);
        osg::AnimationPath *createScanningAnimationPathByType(Type type) const;

        // Hemisphere
        void setHemisphereCoverage(float radius = 1.0f, int slices = 30, int stacks = 15);
        void setHemisphereScanning(float radius = 1.0f, int slices = 30, int stacks = 15);
        osg::AnimationPath *createHemisphereScanningAnimationPath() const;

        // ConcaveHemisphere
        void setConcaveHemisphereCoverage(float radius = 1.0f, int slices = 30, int stacks = 15);
        void setConcaveHemisphereScanning(float radius = 1.0f, int slices = 30, int stacks = 15);
        osg::AnimationPath *createConcaveHemisphereScanningAnimationPath() const;

        osg::Vec4 gradientColor(osg::Vec4 &a, osg::Vec4 &b, double i, double steps) const;
        float lerp(float a, float b, float t) const { return a + (b - a) * t; }
        float smoothstep(float edge0, float edge1, float x) const
        {
            x = std::max(0.0f, std::min(1.0f, (x - edge0) / (edge1 - edge0)));
            return x * x * (3.0f - 2.0f * x);
        }
        float smoothInterp(float a, float b, float t) const
        {
            return a + (b - a) * smoothstep(0.0f, 1.0f, t);
        }
        float convexInterpolation(float a, float b, float t, float power = 2.0f) const
        {
            return a + (b - a) * (1.0f - std::pow(1.0f - t, power));
        }
        float concaveInterpolation(float a, float b, float t, float power = 2.0f) const
        {
            return a + (b - a) * std::pow(t, power);
        }

        static osg::ref_ptr<osg::Program> program;
        static void initProgram();
        static int id;
        static std::string generateName();
        static void initEMIToGroup(osg::Group *root, llhRange range);
        static void updateEMI(osg::MatrixTransform* emiMT, llhRange range);
    };
}
