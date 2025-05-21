#include "RadarGenerator.h"

namespace Radar
{
    int RadarGenerator::id = 0;
    osg::ref_ptr<osg::Program> RadarGenerator::program;
    void RadarGenerator::initProgram()
    {
        program = new osg::Program;

        osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
        vertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/radar_generator/radar_generator.vert");
        osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
        fragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/radar_generator/radar_generator.frag");

        program->addShader(vertexShader.get());
        program->addShader(fragmentShader.get());
    }
    std::string RadarGenerator::generateName()
    {
        std::ostringstream oss;
        oss << "Radar" << std::setfill('0') << std::setw(2) << id++;
        return oss.str();
    }
    RadarGenerator::RadarGenerator(Type type, Settings settings) : type(type), radius(settings.radius), slices(settings.slices), stacks(settings.stacks)
    {
        if (!program.valid())
        {
            initProgram();
        }

        setGeometriesByType(type);
    }

    osg::ref_ptr<osg::Group> RadarGenerator::
        addToGroup(osg::Group *root, osg::Camera *camera, llhRange range, VisualParameters parameter) const
    {
        osg::ref_ptr<osg::Group> baseRadar = new osg::Group;

        osg::ref_ptr<osg::Geode> coverageGeode = new osg::Geode;
        osg::ref_ptr<osg::Geode> linesGeode = new osg::Geode;
        osg::ref_ptr<osg::Geode> scanningGeode = new osg::Geode;

        for (int i = 0; i < coverageGeometries.size(); ++i)
        {
            auto &geometry = coverageGeometries[i];
            auto &linesGeometry = linesGeometries[i];

            osg::CopyOp copyOp(osg::CopyOp::SHALLOW_COPY);
            copyOp.setCopyFlags(copyOp.getCopyFlags() | osg::CopyOp::DEEP_COPY_STATESETS);

            osg::ref_ptr<osg::Geometry> copied = dynamic_cast<osg::Geometry *>(geometry->clone(copyOp));
            osg::ref_ptr<osg::Geometry> linesCopied = dynamic_cast<osg::Geometry *>(linesGeometry->clone(copyOp));

            osg::StateSet *geometryStateSet = new osg::StateSet;
            osg::ref_ptr<osg::Uniform> mainColorUniform = new osg::Uniform(osg::Uniform::FLOAT_VEC4, "uMainColor");

            osg::Vec4 color(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, 0.5);
            mainColorUniform->set(color);
            geometryStateSet->addUniform(mainColorUniform.get());

            copied->setStateSet(geometryStateSet);
            linesCopied->setStateSet(geometryStateSet);

            coverageGeode->addDrawable(copied.get());
            linesGeode->addDrawable(linesCopied.get());
        }
        for (auto &geometry : scanningGeometries)
        {
            osg::CopyOp copyOp(osg::CopyOp::SHALLOW_COPY);
            osg::ref_ptr<osg::Geometry> copied = dynamic_cast<osg::Geometry *>(geometry->clone(copyOp));
            scanningGeode->addDrawable(copied.get());
        }

        // Setting coverageGeode's and linesGeode's StateSet and Initial Node Mask
        {
            osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;
            stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
            stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            stateSet->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);

            osg::ref_ptr<osg::Depth> depth = new osg::Depth;
            depth->setWriteMask(false);
            stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);
            osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode;
            if (parameter.polygonMode == PolygonMode::Line)
            {
                coverageGeode->setNodeMask(0x00);
                polyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
            }
            else if (parameter.polygonMode == PolygonMode::Fill)
            {
                linesGeode->setNodeMask(0x00);
                polyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
            }
            stateSet->setAttributeAndModes(polyMode.get(), osg::StateAttribute::ON);
            osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth;
            lineWidth->setWidth(parameter.LineWidth);
            stateSet->setAttributeAndModes(lineWidth.get(), osg::StateAttribute::ON);

            stateSet->setAttributeAndModes(program.get(), osg::StateAttribute::OVERRIDE);
            coverageGeode->setStateSet(stateSet.get());
            linesGeode->setStateSet(stateSet.get());
            stateSet->setDataVariance(osg::Object::DYNAMIC);
        }

        // Setting scanningGeode's StateSet
        {
            osg::StateSet *scanningStateSet = scanningGeode->getOrCreateStateSet();
            scanningStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
            scanningStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            scanningStateSet->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);

            osg::ref_ptr<osg::Depth> depth = new osg::Depth;
            depth->setWriteMask(false);
            scanningStateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);

            // auto polyMode = new osg::PolygonMode;
            // polyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
            // scanningStateSet->setAttributeAndModes(polyMode, osg::StateAttribute::ON);

            // scanningStateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
        }

        // Transform to world coordinate system
        {
            double centerLatitude = (range.maxLatitude + range.minLatitude) / 2.0;
            double centerLongtitude = (range.maxLongtitude + range.minLongtitude) / 2.0;
            double height = range.minHeight;

            double x, y, z;
            llh2xyz_Ellipsoid(centerLatitude, centerLongtitude, height, x, y, z);

            osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
            double size = std::fmin(
                osg::RadiansToDegrees(range.maxLongtitude - range.minLongtitude) * 100000,
                osg::RadiansToDegrees(range.maxLatitude - range.minLatitude) * 111320);

            osg::Matrix scaleM = osg::Matrix::scale(size, size, size);
            osg::Matrix transM = osg::Matrix::translate(x, y, z);
            osg::Matrix rotM = osg::Matrix::rotate(osg::Vec3(0, 0, 1), osg::Vec3(x, y, z));

            mt->setMatrix(scaleM * rotM * transM);
            {
                osg::StateSet *mtStateSet = mt->getOrCreateStateSet();
                // osg::ref_ptr<osg::Uniform> modelViewProjectionUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "ModelViewProjectionMatrix");
                // modelViewProjectionUniform->setUpdateCallback(new osg_3d_vis::ModelViewProjectionMatrixCallback(camera));
                // mtStateSet->addUniform(modelViewProjectionUniform.get());

                osg::ref_ptr<osg::Uniform> modelUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uModelMatrix");
                modelUniform->setUpdateCallback(new osg_3d_vis::ModelMatrixCallback(camera));
                mtStateSet->addUniform(modelUniform.get());
                osg::ref_ptr<osg::Uniform> viewUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uViewMatrix");
                viewUniform->setUpdateCallback(new osg_3d_vis::ViewMatrixCallback(camera));
                mtStateSet->addUniform(viewUniform.get());
                osg::ref_ptr<osg::Uniform> projectionUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "uProjectionMatrix");
                projectionUniform->setUpdateCallback(new osg_3d_vis::ProjectionMatrixCallback(camera));
                mtStateSet->addUniform(projectionUniform.get());

                osg::ref_ptr<osg::Uniform> timeUniform = new osg::Uniform(osg::Uniform::FLOAT, "uTime");
                timeUniform->setUpdateCallback(new osg_3d_vis::TimeUniformCallback());
                mtStateSet->addUniform(timeUniform.get());
            }
            mt->addChild(coverageGeode.get());
            mt->addChild(linesGeode.get());

            if (parameter.bAnimated)
            {
                osg::ref_ptr<osg::MatrixTransform> animatedMt = new osg::MatrixTransform;
                osg::ref_ptr<osg::AnimationPathCallback> apcb = new osg::AnimationPathCallback;
                apcb->setAnimationPath(createScanningAnimationPathByType(type));
                animatedMt->setUpdateCallback(apcb.get());
                animatedMt->addChild(scanningGeode.get());
                mt->addChild(animatedMt.get());
            }

            baseRadar->addChild(mt.get());
        }

        root->addChild(baseRadar.get());

        baseRadar->setName(generateName());
        return baseRadar;
    }
    void RadarGenerator::setGeometriesByType(Type type)
    {
        switch (type)
        {
        case Type::Hemisphere:
            setHemisphereCoverage(radius, slices, stacks);
            setHemisphereScanning(radius, slices, stacks);
            break;
        case Type::ConcaveHemisphere:
            setConcaveHemisphereCoverage(radius, slices, stacks);
            setConcaveHemisphereScanning(radius, slices, stacks);
            break;

        default:
            break;
        }
    }
    osg::AnimationPath *RadarGenerator::createScanningAnimationPathByType(Type type) const
    {

        switch (type)
        {
        case Type::Hemisphere:
            return createHemisphereScanningAnimationPath();
            break;
        case Type::ConcaveHemisphere:
            return createConcaveHemisphereScanningAnimationPath();
            break;
        default:
            break;
        }
        return nullptr;
    }

    void RadarGenerator::setHemisphereCoverage(float radius, int slices, int stacks)
    {
        int section = 2 * stacks / 3.;
        // 半球面
        {
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            osg::ref_ptr<osg::Geometry> linesGeometry = new osg::Geometry;
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
            osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::QUAD_STRIP, 0);
            osg::ref_ptr<osg::DrawElementsUInt> lineIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::LINES, 0);

            float dTheta = osg::PI_2 / stacks;
            float dPhi = osg::PI * 2.0f / slices;
            float offset = 1e-3;

            for (int i = 0; i <= stacks; ++i)
            {
                float theta = i * dTheta;
                for (int j = 0; j < slices; ++j)
                {
                    float phi = j * dPhi;
                    float x1 = radius * sinf(theta) * cosf(phi);
                    float y1 = radius * sinf(theta) * sinf(phi);
                    float z1 = radius * cosf(theta);

                    vertices->push_back(osg::Vec3(x1, y1, z1));
                }
            }

            for (int i = 0; i <= stacks - 1; ++i)
            {
                for (int j = 0; j <= slices; ++j)
                {
                    indices->push_back(i * slices + j % slices);
                    indices->push_back((i + 1) * slices + j % slices);

                    lineIndices->push_back(i * slices + j % slices);
                    lineIndices->push_back((i + 1) * slices + j % slices);

                    lineIndices->push_back((i + 1) * slices + j % slices);
                    lineIndices->push_back((i + 1) * slices + (j + 1) % slices);

                    lineIndices->push_back((i + 1) * slices + (j + 1) % slices);
                    lineIndices->push_back(i * slices + (j + 1) % slices);

                    lineIndices->push_back(i * slices + (j + 1) % slices);
                    lineIndices->push_back(i * slices + j % slices);
                }
            }

            geometry->setVertexArray(vertices);
            colors->push_back(osg::Vec4(0.0, 1.0, 0.0, 0.3));
            geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
            geometry->addPrimitiveSet(indices.get());

            linesGeometry->setVertexArray(vertices);
            linesGeometry->setColorArray(colors, osg::Array::BIND_OVERALL);
            linesGeometry->addPrimitiveSet(lineIndices.get());

            coverageGeometries.push_back(geometry.get());
            linesGeometries.push_back(linesGeometry.get());
        }

        // 底面
        {
            stacks /= 3;
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            osg::ref_ptr<osg::Geometry> linesGeometry = new osg::Geometry;
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
            osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::QUAD_STRIP, 0);
            osg::ref_ptr<osg::DrawElementsUInt> lineIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::LINES, 0);

            float dRadius = radius / stacks;
            float dPhi = osg::PI * 2.0f / slices;
            for (int i = 0; i <= stacks; ++i)
            {
                float r = radius - i * dRadius;
                for (int j = 0; j < slices; ++j)
                {
                    float phi = j * dPhi;
                    float x = r * cosf(phi);
                    float y = r * sinf(phi);
                    float z = 0.0f;
                    vertices->push_back(osg::Vec3(x, y, z));
                }
            }

            for (int i = 0; i < stacks; ++i)
            {
                float r = radius - i * dRadius;
                for (int j = 0; j < slices; ++j)
                {
                    float phi = j * dPhi;
                    float x = r * cosf(phi);
                    float y = r * sinf(phi);
                    float z = 0.0f;
                    vertices->push_back(osg::Vec3(x, y, z));
                }
            }

            for (int i = 0; i < stacks; ++i)
            {
                for (int j = 0; j <= slices; ++j)
                {
                    indices->push_back(i * slices + j % slices);
                    indices->push_back((i + 1) * slices + j % slices);

                    lineIndices->push_back(i * slices + j % slices);
                    lineIndices->push_back((i + 1) * slices + j % slices);

                    lineIndices->push_back((i + 1) * slices + j % slices);
                    lineIndices->push_back((i + 1) * slices + (j + 1) % slices);

                    lineIndices->push_back((i + 1) * slices + (j + 1) % slices);
                    lineIndices->push_back(i * slices + (j + 1) % slices);

                    lineIndices->push_back(i * slices + (j + 1) % slices);
                    lineIndices->push_back(i * slices + j % slices);
                }
            }

            colors->push_back(osg::Vec4(125. / 255., 73. / 255., 206. / 255., 0.3));
            geometry->setVertexArray(vertices);
            geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
            geometry->addPrimitiveSet(indices.get());

            linesGeometry->setVertexArray(vertices);
            linesGeometry->setColorArray(colors, osg::Array::BIND_OVERALL);
            linesGeometry->addPrimitiveSet(lineIndices.get());

            coverageGeometries.push_back(geometry.get());
            linesGeometries.push_back(linesGeometry.get());
        }
    }
    void RadarGenerator::setHemisphereScanning(float radius, int slices, int stacks)
    {
        // 正面
        {
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

            osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::QUADS, 0);

            float dTheta = osg::PI_2 / stacks;
            float dPhi = osg::PI * 2.0f / slices;
            for (int i = 0; i <= stacks; ++i)
            {
                float theta1 = i * dTheta;
                float theta2 = (i + 1) * dTheta;
                for (int j = 0; j <= fmax(1., slices / 15.); ++j)
                {
                    float phi = j * dPhi;
                    float x1 = radius * sinf(theta1) * cosf(phi);
                    float y1 = radius * sinf(theta1) * sinf(phi);
                    float z1 = radius * cosf(theta1);
                    // Add a small offset to avoid z-fighting
                    vertices->push_back(osg::Vec3(x1 + 0.001, y1 + 0.001, z1 + 0.001));
                }
            }
            int scanningSlicesNum = fmax(1., slices / 15.) + 1;
            for (int i = 0; i < stacks; ++i)
            {

                for (int j = 0; j < scanningSlicesNum - 1; ++j)
                {

                    indices->push_back(i * scanningSlicesNum + j);
                    indices->push_back((i + 1) * scanningSlicesNum + j);
                    indices->push_back((i + 1) * scanningSlicesNum + (j + 1));
                    indices->push_back(i * scanningSlicesNum + (j + 1));
                }
            }
            geometry->setVertexArray(vertices);
            colors->push_back(osg::Vec4(0.95, 0.95, 0.5, 0.3));
            geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
            geometry->addPrimitiveSet(indices.get());

            scanningGeometries.push_back(geometry.get());
        }

        // 前侧面
        {
            int j = 0;
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

            osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::TRIANGLE_FAN, 0);
            indices->push_back(0);
            vertices->push_back(osg::Vec3(0, 0, 0));
            float dTheta = osg::PI_2 / stacks;
            float dPhi = osg::PI * 2.0f / slices;

            for (int i = 0; i <= stacks; ++i)
            {
                float theta1 = i * dTheta;

                float phi = j * dPhi;

                float x1 = radius * sinf(theta1) * cosf(phi);
                float y1 = radius * sinf(theta1) * sinf(phi);
                float z1 = radius * cosf(theta1);

                // Add a small offset to avoid z-fighting
                vertices->push_back(osg::Vec3(x1 + 0.001, y1 + 0.001, z1 + 0.001));
                indices->push_back(vertices->size() - 1);
            }

            geometry->setVertexArray(vertices);
            colors->push_back(osg::Vec4(0.95, 0.95, 0.5, 0.3));
            geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
            geometry->addPrimitiveSet(indices.get());
            scanningGeometries.push_back(geometry.get());
        }

        // 后侧面
        {
            int j = fmax(1., slices / 15.);
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

            osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::TRIANGLE_FAN, 0);
            indices->push_back(0);
            vertices->push_back(osg::Vec3(0, 0, 0));
            float dTheta = osg::PI_2 / stacks;
            float dPhi = osg::PI * 2.0f / slices;

            for (int i = 0; i <= stacks; ++i)
            {
                float theta1 = i * dTheta;

                float phi = j * dPhi;

                float x1 = radius * sinf(theta1) * cosf(phi);
                float y1 = radius * sinf(theta1) * sinf(phi);
                float z1 = radius * cosf(theta1);

                // Add a small offset to avoid z-fighting
                vertices->push_back(osg::Vec3(x1 + 0.001, y1 + 0.001, z1 + 0.001));
                indices->push_back(vertices->size() - 1);
            }

            geometry->setVertexArray(vertices);
            colors->push_back(osg::Vec4(0.95, 0.95, 0.5, 0.3));
            geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
            geometry->addPrimitiveSet(indices.get());

            scanningGeometries.push_back(geometry.get());
        }
    }
    osg::AnimationPath *RadarGenerator::createHemisphereScanningAnimationPath() const
    {
        osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;

        path->setLoopMode(osg::AnimationPath::LOOP);

        unsigned int numSamples = slices;
        float deltaTime = 0.25f;
        float time = 0.0f;
        float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.0f * osg::PI;
        float deltaAngle = 2.0f * osg::PI / numSamples;

        for (unsigned int i = 0; i <= numSamples; ++i)
        {
            osg::Vec3 axis(0.0f, 0.0f, 1.0f);
            osg::Matrix rotation = osg::Matrix::rotate(angle, axis);

            osg::AnimationPath::ControlPoint controlPoint;
            controlPoint.setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
            controlPoint.setRotation(rotation.getRotate());
            controlPoint.setScale(osg::Vec3(1.0f, 1.0f, 1.0f));

            path->insert(time, controlPoint);

            time += deltaTime;
            angle += deltaAngle;
        }

        return path.release();
    }

    void RadarGenerator::setConcaveHemisphereCoverage(float radius, int slices, int stacks)
    {
        float dTheta = osg::PI_2 / stacks;
        float dPhi = osg::PI * 2.0f / slices;
        osg::Vec4 colorA = osg::Vec4(1.0f, 0.0f, 0.0f, 0.4f);
        osg::Vec4 colorB = osg::Vec4(0.0f, 1.0f, 0.0f, 0.4f);
        osg::Vec4 colorC = osg::Vec4(0.0f, 0.0f, 1.0f, 0.4f);

        // 开始下陷的层数
        int concaveBeginStack = std::max(1, stacks / 4);
        // 下陷旋转抛物面
        {
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            osg::ref_ptr<osg::Geometry> linesGeometry = new osg::Geometry;
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
            osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::QUAD_STRIP, 0);
            osg::ref_ptr<osg::DrawElementsUInt> lineIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::LINES, 0);

            int geomStackNum = stacks - concaveBeginStack + 1;
            int section = geomStackNum / 1.1;

            float theta = concaveBeginStack * dTheta;
            float c = sinf(theta) / (radius * cosf(theta) * cosf(theta));

            for (int i = concaveBeginStack; i <= stacks; ++i)
            {
                float theta = i * dTheta;
                for (int j = 0; j < slices; ++j)
                {
                    float phi = j * dPhi;
                    float z1 = radius * cosf(theta);
                    float t1 = c * z1 * z1;
                    float x1 = t1 * cosf(phi);
                    float y1 = t1 * sinf(phi);
                    vertices->push_back(osg::Vec3(x1, y1, z1));
                    if (i - concaveBeginStack < section)
                    {
                        colors->push_back(gradientColor(colorA, colorC, i - concaveBeginStack, section));
                    }
                    else
                    {
                        colors->push_back(gradientColor(colorC, colorB, i - concaveBeginStack - section, geomStackNum - section));
                    }
                }
            }

            for (int i = 0; i < geomStackNum - 1; ++i)
            {
                for (int j = 0; j <= slices; ++j)
                {
                    indices->push_back(i * slices + j % slices);
                    indices->push_back((i + 1) * slices + j % slices);

                    lineIndices->push_back(i * slices + j % slices);
                    lineIndices->push_back((i + 1) * slices + j % slices);

                    lineIndices->push_back((i + 1) * slices + j % slices);
                    lineIndices->push_back((i + 1) * slices + (j + 1) % slices);

                    lineIndices->push_back((i + 1) * slices + (j + 1) % slices);
                    lineIndices->push_back(i * slices + (j + 1) % slices);

                    lineIndices->push_back(i * slices + (j + 1) % slices);
                    lineIndices->push_back(i * slices + j % slices);
                }
            }

            geometry->setVertexArray(vertices);
            geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
            geometry->addPrimitiveSet(indices.get());

            linesGeometry->setVertexArray(vertices);
            linesGeometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
            linesGeometry->addPrimitiveSet(lineIndices.get());

            coverageGeometries.push_back(geometry.get());
            linesGeometries.push_back(linesGeometry.get());
        }

        // 开始汇聚的层数
        int convergeBeginStack = stacks - 2;
        // 球面
        {
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            osg::ref_ptr<osg::Geometry> linesGeometry = new osg::Geometry;
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
            osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::QUAD_STRIP, 0);
            osg::ref_ptr<osg::DrawElementsUInt> lineIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::LINES, 0);

            int geomStackNum = convergeBeginStack - concaveBeginStack + 1;
            int section = 2. * geomStackNum / 3.;

            for (int i = concaveBeginStack; i <= convergeBeginStack; ++i)
            {
                float theta = i * dTheta;
                for (int j = 0; j < slices; ++j)
                {

                    float phi = j * dPhi;
                    float x = radius * sinf(theta) * cosf(phi);
                    float y = radius * sinf(theta) * sinf(phi);
                    float z = radius * cosf(theta);
                    vertices->push_back(osg::Vec3(x, y, z));

                    if (i - concaveBeginStack < section)
                    {
                        colors->push_back(gradientColor(colorA, colorB, i - concaveBeginStack, section));
                    }
                    else
                    {
                        colors->push_back(gradientColor(colorB, colorC, i - concaveBeginStack - section, geomStackNum - section));
                    }
                }
            }

            for (int i = 0; i < geomStackNum - 1; ++i)
            {
                for (int j = 0; j <= slices; ++j)
                {
                    indices->push_back(i * slices + j % slices);
                    indices->push_back((i + 1) * slices + j % slices);

                    lineIndices->push_back(i * slices + j % slices);
                    lineIndices->push_back((i + 1) * slices + j % slices);

                    lineIndices->push_back((i + 1) * slices + j % slices);
                    lineIndices->push_back((i + 1) * slices + (j + 1) % slices);

                    lineIndices->push_back((i + 1) * slices + (j + 1) % slices);
                    lineIndices->push_back(i * slices + (j + 1) % slices);

                    lineIndices->push_back(i * slices + (j + 1) % slices);
                    lineIndices->push_back(i * slices + j % slices);
                }
            }

            geometry->setVertexArray(vertices);
            geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
            geometry->addPrimitiveSet(indices.get());

            linesGeometry->setVertexArray(vertices);
            linesGeometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
            linesGeometry->addPrimitiveSet(lineIndices.get());

            coverageGeometries.push_back(geometry.get());
            linesGeometries.push_back(linesGeometry.get());
        }
        // 底部旋转抛物面
        {
            osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
            osg::ref_ptr<osg::Geometry> linesGeometry = new osg::Geometry;
            osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
            osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
            osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::QUAD_STRIP, 0);
            osg::ref_ptr<osg::DrawElementsUInt> lineIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::LINES, 0);
            osg::Vec4 colorA = osg::Vec4(0.0f, 0.0f, 1.0f, 0.4f);
            osg::Vec4 colorB = osg::Vec4(0.8f, 0.0f, 0.8f, 0.4f);

            int geomStackNum = stacks - convergeBeginStack + 1;
            int section = geomStackNum / 2;
            float theta = dTheta * convergeBeginStack;
            float c = cosf(theta) / (radius * sinf(theta) * sinf(theta));
            for (int i = convergeBeginStack; i <= stacks; ++i)
            {
                float theta = i * dTheta;
                for (int j = 0; j < slices; ++j)
                {
                    float phi = j * dPhi;
                    float z = fmax(0, radius * cosf(theta));
                    float t = sqrt(z / c);
                    ;
                    float x = t * cosf(phi);
                    float y = t * sinf(phi);
                    vertices->push_back(osg::Vec3(x, y, z));
                    colors->push_back(gradientColor(colorA, colorB, i - convergeBeginStack, geomStackNum));
                }
            }

            for (int i = 0; i < geomStackNum - 1; ++i)
            {
                for (int j = 0; j <= slices; ++j)
                {
                    indices->push_back(i * slices + j % slices);
                    indices->push_back((i + 1) * slices + j % slices);

                    lineIndices->push_back(i * slices + j % slices);
                    lineIndices->push_back((i + 1) * slices + j % slices);

                    lineIndices->push_back((i + 1) * slices + j % slices);
                    lineIndices->push_back((i + 1) * slices + (j + 1) % slices);

                    lineIndices->push_back((i + 1) * slices + (j + 1) % slices);
                    lineIndices->push_back(i * slices + (j + 1) % slices);

                    lineIndices->push_back(i * slices + (j + 1) % slices);
                    lineIndices->push_back(i * slices + j % slices);
                }
            }

            geometry->setVertexArray(vertices);
            geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
            geometry->addPrimitiveSet(indices.get());

            linesGeometry->setVertexArray(vertices);
            linesGeometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
            linesGeometry->addPrimitiveSet(lineIndices.get());

            coverageGeometries.push_back(geometry.get());
            linesGeometries.push_back(linesGeometry.get());
        }
    }
    void RadarGenerator::setConcaveHemisphereScanning(float radius, int slices, int stacks)
    {
        float dTheta = osg::PI_2 / stacks;
        float dPhi = osg::PI * 2.0f / slices;
        osg::Vec3 offset = osg::Vec3(0.0, 0.0, 0.01);

        int scanningSlices = 2; // 0, 1, 2

        // 正面
        {
            // 上正面
            {
                osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
                osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::QUADS, 0);

                osg::ref_ptr<osg::Vec3Array> coverageVertices = dynamic_cast<osg::Vec3Array *>(coverageGeometries[0]->getVertexArray());

                for (int i = 0; i < stacks && i * slices < coverageVertices->size(); ++i)
                {
                    for (int j = 0; j <= scanningSlices; ++j)
                    {
                        int idx = i * slices + j;

                        osg::Vec3 vertex = (*coverageVertices)[idx] + offset;
                        vertices->push_back(vertex);
                    }
                }

                for (int i = 0; i < stacks - 1 && (i + 1) * (scanningSlices + 1) < vertices->size(); ++i)
                {
                    for (int j = 0; j < scanningSlices; ++j)
                    {
                        indices->push_back(i * (scanningSlices + 1) + j);
                        indices->push_back((i + 1) * (scanningSlices + 1) + j);
                        indices->push_back((i + 1) * (scanningSlices + 1) + (j + 1));
                        indices->push_back(i * (scanningSlices + 1) + (j + 1));
                    }
                }

                geometry->setVertexArray(vertices);
                colors->push_back(osg::Vec4(0.95, 0.95, 0.5, 0.3));
                geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
                geometry->addPrimitiveSet(indices.get());

                scanningGeometries.push_back(geometry.get());
            }
            // 中正面
            {
                osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
                osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::QUADS, 0);

                osg::ref_ptr<osg::Vec3Array> coverageVertices = dynamic_cast<osg::Vec3Array *>(coverageGeometries[1]->getVertexArray());

                for (int i = 0; i < stacks && i * slices < coverageVertices->size(); ++i)
                {
                    for (int j = 0; j <= scanningSlices; ++j)
                    {
                        int idx = i * slices + j;

                        osg::Vec3 vertex = (*coverageVertices)[idx] + offset;
                        vertices->push_back(vertex);
                    }
                }

                for (int i = 0; i < stacks - 1 && (i + 1) * (scanningSlices + 1) < vertices->size(); ++i)
                {
                    for (int j = 0; j < scanningSlices; ++j)
                    {
                        indices->push_back(i * (scanningSlices + 1) + j);
                        indices->push_back((i + 1) * (scanningSlices + 1) + j);
                        indices->push_back((i + 1) * (scanningSlices + 1) + (j + 1));
                        indices->push_back(i * (scanningSlices + 1) + (j + 1));
                    }
                }

                geometry->setVertexArray(vertices);
                colors->push_back(osg::Vec4(0.95, 0.95, 0.5, 0.3));
                geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
                geometry->addPrimitiveSet(indices.get());

                scanningGeometries.push_back(geometry.get());
            }
            // 下正面
            {
                osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
                osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
                osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::QUADS, 0);

                osg::ref_ptr<osg::Vec3Array> coverageVertices = dynamic_cast<osg::Vec3Array *>(coverageGeometries[2]->getVertexArray());

                for (int i = 0; i < stacks && i * slices < coverageVertices->size(); ++i)
                {
                    for (int j = 0; j <= scanningSlices; ++j)
                    {
                        int idx = i * slices + j;

                        osg::Vec3 vertex = (*coverageVertices)[idx] + offset;
                        vertices->push_back(vertex);
                    }
                }

                for (int i = 0; i < stacks - 1 && (i + 1) * (scanningSlices + 1) < vertices->size(); ++i)
                {
                    for (int j = 0; j < scanningSlices; ++j)
                    {
                        indices->push_back(i * (scanningSlices + 1) + j);
                        indices->push_back((i + 1) * (scanningSlices + 1) + j);
                        indices->push_back((i + 1) * (scanningSlices + 1) + (j + 1));
                        indices->push_back(i * (scanningSlices + 1) + (j + 1));
                    }
                }

                geometry->setVertexArray(vertices);
                colors->push_back(osg::Vec4(0.95, 0.95, 0.5, 0.3));
                geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
                geometry->addPrimitiveSet(indices.get());

                scanningGeometries.push_back(geometry.get());
            }
        }

        // 侧面
        {
            // 左侧面
            {
                int j = 0;
                // 左上侧面
                {
                    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
                    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
                    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::TRIANGLE_FAN, 0);

                    osg::ref_ptr<osg::Vec3Array> coverageVertices = dynamic_cast<osg::Vec3Array *>(coverageGeometries[0]->getVertexArray());

                    for (int i = 0; i < stacks && i * slices < coverageVertices->size(); ++i)
                    {
                        int idx = i * slices + j;

                        osg::Vec3 vertex = (*coverageVertices)[idx] + offset;
                        vertices->push_back(vertex);
                    }
                    vertices->push_back(osg::Vec3(0, 0, 0) + offset);
                    indices->push_back(vertices->size() - 1);
                    for (int i = 0; i < stacks - 1 && i < vertices->size() - 1; ++i)
                    {
                        indices->push_back(i);
                    }

                    geometry->setVertexArray(vertices);
                    colors->push_back(osg::Vec4(0.95, 0.95, 0.5, 0.3));
                    geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
                    geometry->addPrimitiveSet(indices.get());

                    scanningGeometries.push_back(geometry.get());
                }
                // 左中侧面
                {
                    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
                    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
                    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::TRIANGLE_FAN, 0);

                    osg::ref_ptr<osg::Vec3Array> coverageVertices = dynamic_cast<osg::Vec3Array *>(coverageGeometries[1]->getVertexArray());

                    for (int i = 0; i < stacks && i * slices < coverageVertices->size(); ++i)
                    {
                        int idx = i * slices + j;

                        osg::Vec3 vertex = (*coverageVertices)[idx] + offset;
                        vertices->push_back(vertex);
                    }
                    vertices->push_back(osg::Vec3(0, 0, 0) + offset);
                    indices->push_back(vertices->size() - 1);
                    for (int i = 0; i < stacks - 1 && i < vertices->size() - 1; ++i)
                    {
                        indices->push_back(i);
                    }

                    geometry->setVertexArray(vertices);
                    colors->push_back(osg::Vec4(0.95, 0.95, 0.5, 0.3));
                    geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
                    geometry->addPrimitiveSet(indices.get());

                    scanningGeometries.push_back(geometry.get());
                }
                // 左下侧面
                {
                    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
                    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
                    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::TRIANGLE_FAN, 0);

                    osg::ref_ptr<osg::Vec3Array> coverageVertices = dynamic_cast<osg::Vec3Array *>(coverageGeometries[2]->getVertexArray());

                    for (int i = 0; i < stacks && i * slices < coverageVertices->size(); ++i)
                    {
                        int idx = i * slices + j;

                        osg::Vec3 vertex = (*coverageVertices)[idx] + offset;
                        vertices->push_back(vertex);
                    }
                    vertices->push_back(osg::Vec3(0, 0, 0) + offset);
                    indices->push_back(vertices->size() - 1);
                    for (int i = 0; i < stacks - 1 && i < vertices->size() - 1; ++i)
                    {
                        indices->push_back(i);
                    }

                    geometry->setVertexArray(vertices);
                    colors->push_back(osg::Vec4(0.95, 0.95, 0.5, 0.3));
                    geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
                    geometry->addPrimitiveSet(indices.get());

                    scanningGeometries.push_back(geometry.get());
                }
            }
            // 右侧面
            {
                int j = scanningSlices;
                for (int geometeryId = 0; geometeryId < coverageGeometries.size(); ++geometeryId)
                {
                    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
                    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
                    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
                    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::Mode::TRIANGLE_FAN, 0);

                    osg::ref_ptr<osg::Vec3Array> coverageVertices = dynamic_cast<osg::Vec3Array *>(coverageGeometries[geometeryId]->getVertexArray());

                    for (int i = 0; i < stacks && i * slices < coverageVertices->size(); ++i)
                    {
                        int idx = i * slices + j;

                        osg::Vec3 vertex = (*coverageVertices)[idx] + offset;
                        vertices->push_back(vertex);
                    }
                    vertices->push_back(osg::Vec3(0, 0, 0) + offset);
                    indices->push_back(vertices->size() - 1);
                    for (int i = 0; i < stacks - 1 && i < vertices->size() - 1; ++i)
                    {
                        indices->push_back(i);
                    }

                    geometry->setVertexArray(vertices);
                    colors->push_back(osg::Vec4(0.95, 0.95, 0.5, 0.3));
                    geometry->setColorArray(colors, osg::Array::BIND_OVERALL);
                    geometry->addPrimitiveSet(indices.get());

                    scanningGeometries.push_back(geometry.get());
                }
            }
        }
    }
    osg::AnimationPath *RadarGenerator::createConcaveHemisphereScanningAnimationPath() const
    {
        osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;

        path->setLoopMode(osg::AnimationPath::LOOP);

        unsigned int numSamples = slices;
        float deltaTime = 0.25f;
        float time = 0.0f;
        float angle = static_cast<float>(std::rand()) / RAND_MAX * 2.0f * osg::PI;
        float deltaAngle = 2.0f * osg::PI / numSamples;

        for (unsigned int i = 0; i <= numSamples; ++i)
        {
            osg::Vec3 axis(0.0f, 0.0f, 1.0f);
            osg::Matrix rotation = osg::Matrix::rotate(angle, axis);

            osg::AnimationPath::ControlPoint controlPoint;
            controlPoint.setPosition(osg::Vec3(0.0f, 0.0f, 0.0f));
            controlPoint.setRotation(rotation.getRotate());
            controlPoint.setScale(osg::Vec3(1.0f, 1.0f, 1.0f));

            path->insert(time, controlPoint);

            time += deltaTime;
            angle += deltaAngle;
        }

        return path.release();
    }

    osg::Vec4 RadarGenerator::gradientColor(osg::Vec4 &a, osg::Vec4 &b, double i, double steps) const
    {
        return a + (b - a) * i / steps;
    }

    void RadarGenerator::setEMI(osg::Group *baseRadar, llhRange range)
    {
        double centerLatitude = (range.maxLatitude + range.minLatitude) / 2.0;
        double centerLongtitude = (range.maxLongtitude + range.minLongtitude) / 2.0;
        double height = (range.minHeight + range.maxHeight) / 2.0;
        double x, y, z;
        llh2xyz_Ellipsoid(centerLatitude, centerLongtitude, height, x, y, z);
        double size = std::fmin(
            osg::RadiansToDegrees(range.maxLongtitude - range.minLongtitude) * 100000,
            osg::RadiansToDegrees(range.maxLatitude - range.minLatitude) * 111320);
        double radius = size;
        osg::Vec4 emiPrarmeters = osg::Vec4(x, y, z, radius);

        if (baseRadar->getNumChildren() <= 1)
        {
            osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;
            stateSet->setDataVariance(osg::Object::DYNAMIC);


            osg::ref_ptr<osg::Uniform> emiPrarmetersUniform= new osg::Uniform(osg::Uniform::FLOAT_VEC4, "uEMIParameters");
            stateSet->addUniform(emiPrarmetersUniform.get());
            emiPrarmetersUniform->set(emiPrarmeters);

            baseRadar->setStateSet(stateSet);
            initEMIToGroup(baseRadar, range);
        }
        else
        {
            osg::StateSet* baseRadarStateSet = baseRadar->getStateSet();
            if(baseRadarStateSet)
            {
                osg::Uniform* emiPrarmetersUniform = baseRadarStateSet->getUniform("uEMIParameters");
                if(emiPrarmetersUniform)
                {
                    emiPrarmetersUniform->set(emiPrarmeters);
                }
            }
            updateEMI(dynamic_cast<osg::MatrixTransform *>(baseRadar->getChild(1)), range);
        }
    }
    void RadarGenerator::initEMIToGroup(osg::Group *root, llhRange range)
    {
        double centerLatitude = (range.maxLatitude + range.minLatitude) / 2.0;
        double centerLongtitude = (range.maxLongtitude + range.minLongtitude) / 2.0;
        double height = (range.minHeight + range.maxHeight) / 2.0;

        double size = std::fmin(
            osg::RadiansToDegrees(range.maxLongtitude - range.minLongtitude) * 100000,
            osg::RadiansToDegrees(range.maxLatitude - range.minLatitude) * 111320);

        osg::ref_ptr<osg::Geode> shpereGeode = new osg::Geode;
        double x, y, z;
        double radius = size * 1.2;
        llh2xyz_Ellipsoid(centerLatitude, centerLongtitude, height, x, y, z);

        osg::ref_ptr<osg::Sphere> shpere = new osg::Sphere(osg::Vec3(0, 0, 0), radius);
        osg::ref_ptr<osg::ShapeDrawable> shpereDrawable = new osg::ShapeDrawable(shpere.get());
        shpereDrawable->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.3f));
        shpereGeode->addDrawable(shpereDrawable.get());
        {
            // 设置状态集以实现半透明效果
            osg::ref_ptr<osg::StateSet> stateset = shpereGeode->getOrCreateStateSet();
            stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
            stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            stateset->setAttributeAndModes(new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA));

            osg::ref_ptr<osg::Depth> depth = new osg::Depth;
            depth->setWriteMask(false);
            stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);
        }

        // Transform to world coordinate system
        osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
        {
            osg::Matrix scaleM = osg::Matrix::scale(1, 1, 0.5);
            osg::Matrix transM = osg::Matrix::translate(x, y, z);
            osg::Matrix rotM = osg::Matrix::rotate(osg::Vec3(0, 0, 1), osg::Vec3(x, y, z));
            mt->setMatrix(scaleM * rotM * transM);
            mt->addChild(shpereGeode.get());
        }

        root->addChild(mt.get());
    }
    void RadarGenerator::updateEMI(osg::MatrixTransform *emiMT, llhRange range)
    {
        double centerLatitude = (range.maxLatitude + range.minLatitude) / 2.0;
        double centerLongtitude = (range.maxLongtitude + range.minLongtitude) / 2.0;
        double height = (range.minHeight + range.maxHeight) / 2.0;
        double size = std::fmin(
            osg::RadiansToDegrees(range.maxLongtitude - range.minLongtitude) * 100000,
            osg::RadiansToDegrees(range.maxLatitude - range.minLatitude) * 111320);

        double x, y, z;
        llh2xyz_Ellipsoid(centerLatitude, centerLongtitude, height, x, y, z);
        osg::Matrix scaleM = osg::Matrix::scale(1, 1, 0.5);
        osg::Matrix transM = osg::Matrix::translate(x, y, z);
        osg::Matrix rotM = osg::Matrix::rotate(osg::Vec3(0, 0, 1), osg::Vec3(x, y, z));

        emiMT->setMatrix(scaleM * rotM * transM);
    }
} // namespace Radar
