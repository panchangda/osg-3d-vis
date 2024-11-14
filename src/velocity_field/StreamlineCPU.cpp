#include "StreamlineCPU.h"

#include <osg/BlendFunc>

#include "streamline_cpu.h"


class DepthCopyCallback : public osg::Camera::DrawCallback {
public:
    DepthCopyCallback(osg::Texture2D *depthTexture) : _depthTexture(depthTexture) {
    }

    virtual void operator()(osg::RenderInfo &renderInfo) const override {
        // 使用 glCopyTexSubImage2D 复制深度缓冲到深度纹理
        glBindTexture(GL_TEXTURE_2D, _depthTexture->getTextureObject(renderInfo.getState()->getContextID())->id());
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, _depthTexture->getTextureWidth(),
                            _depthTexture->getTextureHeight());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

private:
    osg::ref_ptr<osg::Texture2D> _depthTexture;
};


namespace osg_3d_vis {
    StreamLineCPU::StreamLineCPU(osgViewer::Viewer &viewer, const osg::ref_ptr<osg::Group> &root,
                                 const osg::ref_ptr<osg::Camera> &mainCamera, osg_3d_vis::llhRange range) {
        // return ;

        if (dataDim == EDataDimension::D2) {
            this->initFromDatFile(std::string(OSG_3D_VIS_DATA_PREFIX) + "U.DAT",
                                  std::string(OSG_3D_VIS_DATA_PREFIX) + "V.DAT",
                                  range);
        } else if (dataDim == EDataDimension::D3) {
            this->initFromDatFile3D(std::string(OSG_3D_VIS_DATA_PREFIX) + "U3D.DAT",
                                    std::string(OSG_3D_VIS_DATA_PREFIX) + "V3D.DAT",
                                    std::string(OSG_3D_VIS_DATA_PREFIX) + "W3D.DAT",
                                    range,
                                    osg::Vec3i(249, 205, 200));
        }

        this->setRoot(root);
        this->setShaderPath(std::string(OSG_3D_VIS_SHADER_PREFIX) + "streamline-cpu/");

        this->initializeDifferentialMethod();
        this->initializeSmoothAlgorithm();

        this->initializeRandomSeedSelection();
        this->initializeTexturesAndImages();
        this->cb_resizeTexture(mainCamera);
        this->initializeLineColorOptions();
        this->initializeSelectionRGBA();

        this->regenerateRandomPointsAndSteamLines();
        this->initializeLineGeometryRenderState();
        this->initializeArrowGeometryRenderState();

        /* get main camera color & depth buffer */
        mainCamera->attach(osg::Camera::COLOR_BUFFER, this->screenColorTexture);
        // mainCamera->attach(osg::Camera::DEPTH_BUFFER, this->screenDepthTexture);
        // temp way to read back depth buffer
        mainCamera->setPostDrawCallback(new DepthCopyCallback(screenDepthTexture));
        /* multi-pass creation */
        // use slave camera for segmentdraw to ensure following main camera
        // osg::ref_ptr<osg::Camera> slaveCamera = this->createSegmentDrawPass(mainCamera);
        // slaveCamera->setGraphicsContext(mainCamera->getGraphicsContext());
        // viewer.addSlave(slaveCamera, false);
        root->addChild(this->createSegmentDrawPass(mainCamera));
        root->addChild(this->createTrailDrawPass());
        root->addChild(this->createScreenDrawPass());
        root->addChild(this->createCopyPass());

        root->addChild(this->createTextPass());

        /* add pickHandler to viewer */
        viewer.addEventHandler(new PickHandler(this));

        /* connect to ui */
        auto wnd = new streamline_cpu();
        wnd->setStreamLineConnections(this);
        wnd->show();
    }

    void StreamLineCPU::initFromDatFile(std::string str1, std::string str2, osg_3d_vis::llhRange range) {
        std::ifstream fs(str1, std::ios::binary);
        if (!fs.is_open()) std::cout << "Cannot open file from path: " << "str1" << std::endl;
        float temp;
        float _u[52000];
        float _v[52000];
        fs.read((char *) &temp, sizeof(float));
        fs.read((char *) _u, sizeof(float) * 52000);
        fs.close();

        fs.open(str2, std::ios::binary);
        if (!fs.is_open()) std::cout << "Cannot open file from path: " << "str2" << std::endl;
        fs.read((char *) &temp, sizeof(float));
        fs.read((char *) _v, sizeof(float) * 52000);
        fs.close();
        for (int i = 0; i < 52000; i++) {
            u.push_back(_u[i]);
            v.push_back(_v[i]);
        }


        minY = 99.0f;
        maxY = 150.f;

        minX = -10.0f;
        maxX = 52.0f;

        dimY = 205;
        dimX = 249;

        dx = (maxX - minX) / (dimX - 1);
        dy = (maxY - minY) / (dimY - 1);

        idx = 0;
        pointDensity = 5.0f;
        h = 0.5f;
        speedScaleFactor = 0.5f;
        minLineLength = 10;
        pointsSum = int(dimX * dimY / pointDensity);
        linesSum = pointsSum;
        lines.resize(linesSum);
        colors.resize(linesSum);
        speeds.resize(linesSum);

        geometry = new osg::Geometry;
        geometry->setDataVariance(osg::Object::DYNAMIC);


        linePoints = new osg::Vec3Array;
        lineColors = new osg::Vec4Array;
        linePoints->resize(linesSum * 2 * 3, osg::Vec3(0.0, 0.0, 0.0));
        lineColors->resize(linesSum * 2 * 3, osg::Vec4());
        linesDrawArray = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, pointsSum * 2);


        leftArrows.resize(linesSum);
        rightArrows.resize(linesSum);
        arrowGeometry = new osg::Geometry;
        arrowGeometry->setDataVariance(osg::Object::DYNAMIC);
        arrowPoints = new osg::Vec3Array;
        arrowPoints->resize(linesSum * 4, osg::Vec3(0.0f, 0.0, 0.0));
        arrowColors = new osg::Vec4Array();
        arrowColors->resize(linesSum * 4);
        arrowColors->setDataVariance(osg::Object::DYNAMIC);
        arrowsDrawArray = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, pointsSum * 4);

        arrowUseLineColor = false;
        showArrow = false;
        arrowRGBA = osg::Vec4(0.0, 0.0, 0.0, 1.0);
    }

    void loadDataFromFile(const std::string &filePath, std::vector<float> &data) {
        std::ifstream fs(filePath, std::ios::binary);
        if (!fs.is_open()) {
            std::cerr << "Cannot open file from path: " << filePath << std::endl;
            return;
        }

        // 跳过第一个占位float
        float temp;
        fs.read(reinterpret_cast<char *>(&temp), sizeof(float));

        // 直接读取整个数据块
        fs.read(reinterpret_cast<char *>(data.data()), sizeof(float) * data.size());
        fs.close();
    }

    void StreamLineCPU::initFromDatFile3D(std::string uFilePath, std::string vFilePath, std::string wFilePath,
                                          osg_3d_vis::llhRange range, osg::Vec3i dataDim) {
        // dimX = 249;
        // dimY = 205;
        // dimZ = 200;

        dimY = dataDim.y();
        dimX = dataDim.x();
        dimZ = dataDim.z();

        int DataPointSum = dimX * dimY * dimZ;

        u.resize(DataPointSum);
        v.resize(DataPointSum);
        w.resize(DataPointSum);

        // 读取每个数据文件
        loadDataFromFile(uFilePath, u);
        loadDataFromFile(vFilePath, v);
        loadDataFromFile(wFilePath, w);


        minY = 99.0f;
        maxY = 150.f;

        minX = -10.0f;
        maxX = 52.0f;

        minZ = 0.0f;
        maxZ = 500000.0f;


        dx = (maxX - minX) / (dimX - 1);
        dy = (maxY - minY) / (dimY - 1);
        dz = (maxZ - minZ) / (dimZ - 1);

        idx = 0;
        pointDensity = 500.0f;
        h = 0.5f;
        speedScaleFactor = 0.5f;
        minLineLength = 10;
        pointsSum = int(dimX * dimY * dimZ / pointDensity);
        linesSum = pointsSum;
        lines.resize(linesSum);
        // for(auto &line : lines) {
        // 	line.resize(minLineLength);
        // }
        colors.resize(linesSum);
        // for(auto &lineColor : colors) {
        // 	lineColor.resize(minLineLength);
        // }
        speeds.resize(linesSum);


        geometry = new osg::Geometry;
        geometry->setDataVariance(osg::Object::DYNAMIC);


        linePoints = new osg::Vec3Array;
        lineColors = new osg::Vec4Array;
        // 2: start point + end point
        // 3: ???
        linePoints->resize(linesSum * 2 * 3, osg::Vec3(0.0, 0.0, 0.0));
        lineColors->resize(linesSum * 2 * 3, osg::Vec4());
        linesDrawArray = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, pointsSum * 2);


        leftArrows.resize(linesSum);
        rightArrows.resize(linesSum);
        arrowGeometry = new osg::Geometry;
        arrowGeometry->setDataVariance(osg::Object::DYNAMIC);
        arrowPoints = new osg::Vec3Array;
        arrowPoints->resize(linesSum * 4, osg::Vec3(0.0f, 0.0, 0.0));
        arrowColors = new osg::Vec4Array();
        arrowColors->resize(linesSum * 4);
        arrowColors->setDataVariance(osg::Object::DYNAMIC);
        arrowsDrawArray = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, pointsSum * 4);

        arrowUseLineColor = false;
        showArrow = false;
        arrowRGBA = osg::Vec4(0.0, 0.0, 0.0, 1.0);
    }

    void StreamLineCPU::initializeTexturesAndImages() {
        rttTextureSize = 2048;


        screenColorTexture = new osg::Texture2D;
        screenColorTexture->setSourceFormat(GL_RGBA);
        screenColorTexture->setInternalFormat(GL_RGBA32F_ARB);
        screenColorTexture->setSourceType(GL_FLOAT);
        screenColorTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        screenColorTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

        segmentColorTexture = new osg::Texture2D;
        segmentColorTexture->setSourceFormat(GL_RGBA);
        segmentColorTexture->setInternalFormat(GL_RGBA32F_ARB);
        segmentColorTexture->setSourceType(GL_FLOAT);
        segmentColorTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        segmentColorTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

        currTrailColorTexture = new osg::Texture2D;
        currTrailColorTexture->setDataVariance(osg::Object::DYNAMIC);
        currTrailColorTexture->setSourceFormat(GL_RGBA);
        currTrailColorTexture->setInternalFormat(GL_RGBA32F_ARB);
        currTrailColorTexture->setSourceType(GL_FLOAT);
        currTrailColorTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        currTrailColorTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

        nextTrailColorTexture = new osg::Texture2D;
        nextTrailColorTexture->setDataVariance(osg::Object::DYNAMIC);
        nextTrailColorTexture->setSourceFormat(GL_RGBA);
        nextTrailColorTexture->setInternalFormat(GL_RGBA32F_ARB);
        nextTrailColorTexture->setSourceType(GL_FLOAT);
        nextTrailColorTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        nextTrailColorTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

        float *translucentColors = new float[sizeof(float) * 4 * this->rttTextureSize * this->rttTextureSize];
        for (int i = 0; i < this->rttTextureSize * this->rttTextureSize; i++) {
            translucentColors[i * 4 + 0] = 0.0f;
            translucentColors[i * 4 + 1] = 0.0f;
            translucentColors[i * 4 + 2] = 0.0f;
            translucentColors[i * 4 + 3] = 0.0f;
        }
        osg::ref_ptr<osg::Image> translucentImage = new osg::Image();
        translucentImage->setImage(rttTextureSize, rttTextureSize, 1, GL_RGBA, GL_RGBA, GL_FLOAT,
                                   (unsigned char *) translucentColors, osg::Image::USE_NEW_DELETE);

        nextTrailColorTexture->setImage(translucentImage);
        //currTrailColorTexture->setImage(translucentImage);

        screenDepthTexture = new osg::Texture2D;
        screenDepthTexture->setSourceFormat(GL_DEPTH_COMPONENT);
        screenDepthTexture->setSourceType(GL_FLOAT);
        screenDepthTexture->setInternalFormat(GL_DEPTH_COMPONENT);
        screenDepthTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        screenDepthTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);


        segmentDepthTexture = new osg::Texture2D;
        segmentDepthTexture->setSourceFormat(GL_DEPTH_COMPONENT);
        segmentDepthTexture->setSourceType(GL_FLOAT);
        segmentDepthTexture->setInternalFormat(GL_DEPTH_COMPONENT);
        segmentDepthTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        segmentDepthTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);


        currTrailDepthTexture = new osg::Texture2D;
        currTrailDepthTexture->setSourceFormat(GL_DEPTH_COMPONENT);
        currTrailDepthTexture->setSourceType(GL_FLOAT);
        currTrailDepthTexture->setInternalFormat(GL_DEPTH_COMPONENT);
        currTrailDepthTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        currTrailDepthTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);


        nextTrailDepthTexture = new osg::Texture2D;
        nextTrailDepthTexture->setSourceFormat(GL_DEPTH_COMPONENT);
        nextTrailDepthTexture->setSourceType(GL_FLOAT);
        nextTrailDepthTexture->setInternalFormat(GL_DEPTH_COMPONENT);
        nextTrailDepthTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
        nextTrailDepthTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
    }

    void StreamLineCPU::cb_resizeTexture(osg::Camera *mainCamera) {
        screenDepthTexture->setTextureWidth(mainCamera->getViewport()->width());
        screenDepthTexture->setTextureHeight(mainCamera->getViewport()->height());
        ViewerMainCamera = mainCamera;
    }

    void StreamLineCPU::initializeLineGeometryRenderState() {
        //geometry->setVertexArray(linePoints.get());
        //geometry->setColorArray(lineColors.get());
        //geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

        geometry->setVertexAttribArray(0, linePoints.get(), osg::Array::BIND_PER_VERTEX);
        geometry->setVertexAttribArray(1, lineColors.get(), osg::Array::BIND_PER_VERTEX);
        geometry->addPrimitiveSet(linesDrawArray.get());
        geometry->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(3.0f));

        //geometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
        //geometry->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    }

    void StreamLineCPU::initializeArrowGeometryRenderState() {
        arrowGeometry->setVertexArray(arrowPoints.get());
        arrowGeometry->setColorArray(arrowColors.get());
        arrowGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        arrowGeometry->addPrimitiveSet(arrowsDrawArray.get());
        root->addChild(arrowGeometry);
    }

    void StreamLineCPU::regenerateRandomPointsAndSteamLines() {
        lines.clear();
        colors.clear();
        speeds.clear();
        leftArrows.clear();
        rightArrows.clear();

        lines.resize(linesSum);
        colors.resize(linesSum);
        speeds.resize(linesSum);
        leftArrows.resize(linesSum);
        rightArrows.resize(linesSum);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distrib(0.0, 1.0);

        for (int i = 0; i < pointsSum; i++) {
            float randX = distrib(gen);
            float randY = distrib(gen);
            float randZ = distrib(gen);
            switch (randomSeedSelectionAlgorithm) {
                case PSEUDO_RANDOM: {
                    randX = minX + (maxX - minX) * randX;
                    randY = minY + (maxY - minY) * randY;
                    randZ = minZ + (maxZ - minZ) * randZ;
                    break;
                }
                case DENSITY_BASED: {
                    float randTmp = distrib(gen) * (samplerLength - 1);
                    if (dataDim == EDataDimension::D2) {
                        float xStart = densityWeightedTile[(int) randTmp * 3];
                        float yStart = densityWeightedTile[(int) randTmp * 3 + 1];
                        randX = xStart + randX * tileDivideXInterval;
                        randY = yStart + randY * tileDivideYInterval;
                        randX = minX + (maxX - minX) * randX;
                        randY = minY + (maxY - minY) * randY;
                    } else if (dataDim == EDataDimension::D3) {
                        float xStart = densityWeightedTile[(int) randTmp * 3];
                        float yStart = densityWeightedTile[(int) randTmp * 3 + 1];
                        float zStart = densityWeightedTile[(int) randTmp * 3 + 2];
                        randX = xStart + randX * tileDivideXInterval;
                        randY = yStart + randY * tileDivideYInterval;
                        randZ = zStart + randZ * tileDivideZInterval;
                        randX = minX + (maxX - minX) * randX;
                        randY = minY + (maxY - minY) * randY;
                        randZ = minZ + (maxZ - minZ) * randZ;
                    }

                    break;
                }
                case INFORMATION_ENTROPY_BASED: {
                    float randTmp = distrib(gen) * samplerLength;
                    if (dataDim == EDataDimension::D2) {
                        float xStart = infoEntropyWeightedTile[(int) randTmp * 3];
                        float yStart = infoEntropyWeightedTile[(int) randTmp * 3 + 1];
                        randX = xStart + randX * tileDivideXInterval;
                        randY = yStart + randY * tileDivideYInterval;
                        randX = minX + (maxX - minX) * randX;
                        randY = minY + (maxY - minY) * randY;
                    } else if (dataDim == EDataDimension::D3) {
                        float xStart = infoEntropyWeightedTile[(int) randTmp * 3];
                        float yStart = infoEntropyWeightedTile[(int) randTmp * 3 + 1];
                        float zStart = infoEntropyWeightedTile[(int) randTmp * 3 + 2];
                        randX = xStart + randX * tileDivideXInterval;
                        randY = yStart + randY * tileDivideYInterval;
                        randZ = zStart + randZ * tileDivideZInterval;
                        randX = minX + (maxX - minX) * randX;
                        randY = minY + (maxY - minY) * randY;
                        randZ = minZ + (maxZ - minZ) * randZ;
                    }
                    break;
                }
                default: {
                    randX = minX + (maxX - minX) * randX;
                    randY = minY + (maxY - minY) * randY;
                    randZ = minZ + (maxZ - minZ) * randZ;
                    break;
                }
            }

            //std::cout << randX << ", " << randY << std::endl;

            if (dataDim == EDataDimension::D2) {
                makeStreamLine(i, randX, randY, 0.0, geometry, linePoints, lineColors);
            } else if (dataDim == EDataDimension::D3) {
                makeStreamLine(i, randX, randY, randZ, geometry, linePoints, lineColors);
            }
        }


        std::cout << "randomPoints generation completed!" << std::endl;
        updateGeometry(0);
    }

    osg::Vec2 StreamLineCPU::Rotate(osg::Vec2 v, float theta) {
        return osg::Vec2(v.x() * cos(theta) - v.y() * cos(theta), v.x() * sin(theta) + v.y() * cos(theta));
    }

    osg::Vec2 StreamLineCPU::constructCircularArc(float t, osg::Vec2 prevPosition, osg::Vec2 currentPosition,
                                                  osg::Vec2 nextPosition) {
        float h = 0.5f;
        osg::Vec2 A = osg::Vec2(prevPosition.x(), prevPosition.y());
        osg::Vec2 B = osg::Vec2(nextPosition.x(), nextPosition.y());
        osg::Vec2 midpoint = osg::Vec2((A.x() + B.x()) / 2, (A.y() + B.y()) / 2);
        osg::Vec2 perpendicular = osg::Vec2(-(B.y() - midpoint.y()), (B.x() - midpoint.x()));
        osg::Vec2 H = midpoint + perpendicular * h;
        osg::Vec2 center = midpoint - perpendicular / h;
        float radius = std::sqrt(pow(A.x() - center.x(), 2) + pow(A.y() - center.y(), 2));
        float startAngle = std::atan2(A.y() - center.y(), A.x() - center.x());
        float endAngle = std::atan2(B.y() - center.y(), B.x() - center.x());
        float angleDelta = endAngle - startAngle;

        if (angleDelta * h > 0) {
            angleDelta -= (h > 0) ? 2 * osg::PI : -2 * osg::PI;
        }
        float angle = startAngle + angleDelta * t;
        float x = center.x() + radius * std::cos(angle);
        float y = center.y() + radius * std::sin(angle);
        return osg::Vec2(x, y);
    }

    osg::Vec2 StreamLineCPU::constructBezierCurve(float t, osg::Vec2 prevPosition, osg::Vec2 currentPosition,
                                                  osg::Vec2 nextPosition) {
        float u = 1.0 - t;
        float tt = t * t;
        float uu = u * u;
        float ut = u * t;

        osg::Vec2 p0 = osg::Vec2(prevPosition.x(), prevPosition.y());
        osg::Vec2 p1 = osg::Vec2(currentPosition.x(), currentPosition.y());
        osg::Vec2 p2 = osg::Vec2(nextPosition.x(), nextPosition.y());
        osg::Vec2 p;
        p.x() = uu * p0.x() + 2 * ut * p1.x() + tt * p2.x();
        p.y() = uu * p0.y() + 2 * ut * p1.y() + tt * p2.y();
        return osg::Vec2(p.x(), p.y());
    }

    // Actually is CubicSpline
    osg::Vec3 StreamLineCPU::constructCircularArc3D(float t, osg::Vec3 prevPosition, osg::Vec3 currentPosition,
                                                    osg::Vec3 nextPosition) {
        osg::Vec3 p0 = prevPosition;
        osg::Vec3 p1 = currentPosition;
        osg::Vec3 p2 = nextPosition;

        // 控制点可以根据需要进行调整
        osg::Vec3 control1 = p0 + (p1 - p0) * 0.5;
        osg::Vec3 control2 = p1 + (p2 - p1) * 0.5;

        // 插值公式
        float u = 1.0f - t;
        osg::Vec3 point =
                p0 * u * u * u +
                control1 * 3 * u * u * t +
                control2 * 3 * u * t * t +
                p2 * t * t * t;

        return point;

        return point;
    }

    osg::Vec3 StreamLineCPU::constructBezierCurve3D(float t, osg::Vec3 prevPosition, osg::Vec3 currentPosition,
                                                    osg::Vec3 nextPosition) {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float ut = u * t;

        osg::Vec3 p0 = prevPosition;
        osg::Vec3 p1 = currentPosition;
        osg::Vec3 p2 = nextPosition;
        osg::Vec3 p;
        p[0] = uu * p0.x() + 2 * ut * p1.x() + tt * p2.x();
        p[1] = uu * p0.y() + 2 * ut * p1.y() + tt * p2.y();
        p[2] = uu * p0.z() + 2 * ut * p1.z() + tt * p2.z();

        return {p.x(), p.y(), p.z()};
    }

    osg::Vec3 StreamLineCPU::calculateSpeed(osg::Vec3 y_n) {
        osg::Vec3 speed = osg::Vec3();
        switch (differentialMethod) {
            case FORWARD_EULER: {
                osg::Vec3 f_n = linearInterpolation(y_n);
                speed = f_n * h * speedScaleFactor;
                break;
            }

            case RUNGE_KUTTA: {
                osg::Vec3 f_n = linearInterpolation(y_n);

                osg::Vec3 midpoint = y_n + f_n * 0.5 * h * speedScaleFactor;

                speed = linearInterpolation(midpoint) * h * speedScaleFactor;

                break;
            }
            default:
                speed = osg::Vec3();
        }
        return speed;
    }

    // write line point, speed point, arrow point, line color
    void StreamLineCPU::processPosAndSpeed(int index, osg::Vec3 nextPos, osg::Vec3 speed) {
        double x, y, z;
        llh2xyz_Ellipsoid(osg::DegreesToRadians(nextPos.x()),
                          osg::DegreesToRadians(nextPos.y() + 180.0),
                          1000.0f + nextPos.z(),
                          x, y, z);
        lines[index].push_back(osg::Vec3(x, y, z));
        /* transform  [lon&lat] --> [0, 1] */
        //lines[index].push_back(osg::Vec3(
        //	(nextPos.x() - minX)/(maxX - minX),
        //	(nextPos.y() - minY)/(maxY-minY),
        //	0.0));
        colors[index].push_back(osg::Vec4(
            hsvToRgb(osg::Vec3(getHueByVector(osg::Vec2(speed[0], speed[1]), hueRange, hueOffset), hsvSaturation,
                               hsvValue)), 1.0f));
        speeds[index].push_back(speed);


        /* calculate arrows points */
        osg::Vec2 dir = osg::Vec2(speed[0], speed[1]);
        dir.normalize();
        osg::Vec2 lArrowDir = Rotate(-dir, osg::PI / 6.0) * 0.1f;
        osg::Vec2 rArrowDir = Rotate(-dir, -osg::PI / 6.0) * 0.1f;
        osg::Vec2 lArrow = osg::Vec2(nextPos.x() + lArrowDir.x(), nextPos.y() + lArrowDir.y());
        osg::Vec2 rArrow = osg::Vec2(nextPos.x() + rArrowDir.x(), nextPos.y() + rArrowDir.y());
        llh2xyz_Ellipsoid(osg::DegreesToRadians(lArrow.x()),
                          osg::DegreesToRadians(lArrow.y() + 180.0),
                          1000.0f + nextPos.z(), x, y, z);
        leftArrows[index].push_back(osg::Vec3(x, y, z));
        llh2xyz_Ellipsoid(osg::DegreesToRadians(rArrow.x()),
                          osg::DegreesToRadians(rArrow.y() + 180.0),
                          1000.0f + nextPos.z(), x, y, z);
        rightArrows[index].push_back(osg::Vec3(x, y, z));
    }

    void StreamLineCPU::regenerateLineColors() {
        for (int i = 0; i < speeds.size(); i++) {
            for (int j = 0; j < speeds[i].size(); j++) {
                osg::Vec3 tmpSpeed = speeds[i][j];
                colors[i][j] = osg::Vec4(osg::Vec4(
                    hsvToRgb(osg::Vec3(getHueByVector(osg::Vec2(tmpSpeed[0], tmpSpeed[1]), hueRange, hueOffset),
                                       hsvSaturation, hsvValue)), 1.0f));
            }
        }
    }


    /*
     * while(len < minLineLength || random decay not kill)
     * 1. calculate speed
     * 2. generate nextPos, line, arrow
     * 3. apply smooth algorithm
     */
    void StreamLineCPU::makeStreamLine(int index, float x0, float y0, float z0, osg::ref_ptr<osg::Geometry> geometry,
                                       osg::ref_ptr<osg::Vec3Array> linePoints,
                                       osg::ref_ptr<osg::Vec4Array> lineColors) {
        // Runge-Kutta Second-order methods with two stages
        // aka midpoint method
        osg::Vec3 y_n = osg::Vec3(x0, y0, z0);
        osg::Vec3 speed;
        int tmpLength = 0;
        //for (int i = 0; i < lineLength; i++) {
        //	osg::Vec3 f_n = linearInterpolation(y_n);
        //	osg::Vec3 midpoint = y_n + f_n * 0.5 * h * speedScaleFactor;
        //	speed = linearInterpolation(midpoint) * h * speedScaleFactor;
        //	// check if is land
        //	if (speed.x() == 0 || speed.y() == 0) return;
        //	double x, y, z;
        //	osg::Vec3 nextPos = y_n + speed;
        //	llh2xyz_Ellipsoid(osg::DegreesToRadians(nextPos.x()), osg::DegreesToRadians(nextPos.y() + 180.0), 1000, x, y, z);
        //	lines[index].push_back(osg::Vec3(x, y, z));
        //	colors[index].push_back(hsvToRgb(osg::Vec3(getHueByVector(osg::Vec2(speed[0], speed[1])), 0.75f, 1.0f)));
        //	y_n += speed;
        //}

        /* Random Length Segment Generation */
        float dropBase = 0.03f;
        float dropBump = 0.01f;
        while (1) {
            speed = calculateSpeed(y_n);
            /* if is land or if inside border */
            if (speed.x() == 0 || speed.y() == 0) return;
            osg::Vec3 nextPos = y_n + speed;
            if (nextPos.x() >= maxX || nextPos.y() >= maxY) return;

            if (this->smoothAlgorithm == STRAIGHT_LINE) {
                processPosAndSpeed(index, nextPos, speed);
            } else if (this->smoothAlgorithm == CIRCULAR_ARC) {
                osg::Vec3 prevPos = y_n;
                osg::Vec3 currPos = nextPos;
                y_n = currPos;
                speed = calculateSpeed(y_n);
                // check if is land
                if (speed.x() == 0 || speed.y() == 0) return;
                nextPos = y_n + speed;
                if (nextPos.x() >= maxX || nextPos.y() >= maxY) return;
                osg::Vec3 smoothedPoint;
                for (float samplePoint = 0.0f; samplePoint <= 1.0f; samplePoint += lineSegmentSubStep) {
                    if (samplePoint == 0.0f) {
                        smoothedPoint = prevPos;
                    } else if (samplePoint == 1.0f) {
                        smoothedPoint = nextPos;
                    } else {
                        if (dataDim == EDataDimension::D2) {
                            smoothedPoint = osg::Vec3(constructCircularArc(samplePoint,
                                                                           osg::Vec2(prevPos.x(), prevPos.y()),
                                                                           osg::Vec2(currPos.x(), currPos.y()),
                                                                           osg::Vec2(nextPos.x(), nextPos.y())
                                                      ), 0.0);
                        } else if (dataDim == EDataDimension::D3) {
                            smoothedPoint = constructCircularArc3D(samplePoint, prevPos, currPos, nextPos);
                        }
                    }

                    processPosAndSpeed(index, smoothedPoint, speed);
                }
            } else if (this->smoothAlgorithm == BEZIER_CURVE) {
                osg::Vec3 prevPos = y_n;
                osg::Vec3 currPos = nextPos;
                y_n = currPos;
                speed = calculateSpeed(y_n);
                // check if is land
                if (speed.x() == 0 || speed.y() == 0) return;
                nextPos = y_n + speed;
                if (nextPos.x() >= maxX || nextPos.y() >= maxY) return;
                osg::Vec3 smoothedPoint;
                for (float samplePoint = 0.0f; samplePoint <= 1.0f; samplePoint += lineSegmentSubStep) {
                    if (samplePoint == 0.0f) {
                        smoothedPoint = prevPos;
                    } else if (samplePoint == 1.0f) {
                        smoothedPoint = nextPos;
                    } else {
                        if (dataDim == EDataDimension::D2) {
                            smoothedPoint = osg::Vec3(constructBezierCurve(samplePoint,
                                                                           osg::Vec2(prevPos.x(), prevPos.y()),
                                                                           osg::Vec2(currPos.x(), currPos.y()),
                                                                           osg::Vec2(nextPos.x(), nextPos.y())
                                                      ), 0.0);
                        } else if (dataDim == EDataDimension::D3) {
                            smoothedPoint = constructBezierCurve3D(samplePoint, prevPos, currPos, nextPos);
                        }
                    }

                    processPosAndSpeed(index, smoothedPoint, speed);
                }
            }

            y_n = nextPos;

            /* ensure minimum line length */
            tmpLength++;
            if (tmpLength < minLineLength)
                continue;
            else {
                /* enable random decay to stop line make randomly*/
                float randSeed = rand() / (float) RAND_MAX;
                if (randSeed < dropBase + dropBump * speed.x() + dropBase * speed.y()) return;
            }
        }
    }

    void StreamLineCPU::updateGeometry(int time_t) {
        selectedAvgSpeed = osg::Vec3();
        for (int i = 0; i < linesSum; i++) {
            if (lines[i].size() == 0 || lines[i].size() == 1) {
                (*linePoints)[i * 2].set(osg::Vec3(0.0, 0.0, 0.0));
                (*linePoints)[i * 2 + 1].set(osg::Vec3(0.0, 0.0, 0.0));
                (*lineColors)[i * 2].set(1.0, 1.0, 1.0, 1.0);
                (*lineColors)[i * 2 + 1].set(1.0, 1.0, 1.0, 1.0);

                continue;
            }
            int lineLength = lines[i].size();
            int idx = time_t % lineLength + 1;
            if (idx >= lineLength || idx < 1) {
                // std::cout << idx << " " << lineLength << std::endl;
                idx = 1;
            }
            (*linePoints)[i * 2].set(lines[i][idx - 1]);
            (*linePoints)[i * 2 + 1].set(lines[i][idx]);

            osg::Vec4 c0 = colors[i][idx - 1];
            osg::Vec4 c1 = colors[i][idx];

            if (lineUseSameColor) {
                c0 = c1 = lineRGBA;
            }

            if (selected[i] == true && selectedLineCount > 0) {
                c0 = c1 = selectionRGBA;
                selectedAvgSpeed += speeds[i][idx-1] / (float) selectedLineCount;
            }

            (*lineColors)[i * 2].set(c0[0], c0[1], c0[2], c0[3]);
            (*lineColors)[i * 2 + 1].set(c1[0], c1[1], c1[2], c1[3]);


            /* Arrow Geometry Update*/
            if (showArrow) {
                // calculate arrows dir
                (*arrowPoints)[i * 4].set(lines[i][idx]);
                (*arrowPoints)[i * 4 + 1].set(leftArrows[i][idx]);
                (*arrowPoints)[i * 4 + 2].set(lines[i][idx]);
                (*arrowPoints)[i * 4 + 3].set(rightArrows[i][idx]);

                if (arrowUseLineColor) {
                    (*arrowColors)[i * 4].set(c1[0], c1[1], c1[2], c1[3]);
                    (*arrowColors)[i * 4 + 1].set(c1[0], c1[1], c1[2], c1[3]);
                    (*arrowColors)[i * 4 + 2].set(c1[0], c1[1], c1[2], c1[3]);
                    (*arrowColors)[i * 4 + 3].set(c1[0], c1[1], c1[2], c1[3]);
                } else {
                    (*arrowColors)[i * 4].set(arrowRGBA[0], arrowRGBA[1], arrowRGBA[2], arrowRGBA[3]);
                    (*arrowColors)[i * 4 + 1].set(arrowRGBA[0], arrowRGBA[1], arrowRGBA[2], arrowRGBA[3]);
                    (*arrowColors)[i * 4 + 2].set(arrowRGBA[0], arrowRGBA[1], arrowRGBA[2], arrowRGBA[3]);
                    (*arrowColors)[i * 4 + 3].set(arrowRGBA[0], arrowRGBA[1], arrowRGBA[2], arrowRGBA[3]);
                }
            }
        }

        if(selectedLineCount > 0) {
            LineAvgSpeedText ="average speed:("
            + std::to_string(selectedAvgSpeed.x())
            + "," + std::to_string(selectedAvgSpeed.y())
            + "," + std::to_string(selectedAvgSpeed.z())
            + ")";
            streamlineText->setText(LineCountText + LineAvgSpeedText);
        }
        if (showArrow) {
            arrowsDrawArray->setCount(linesSum * 4);
            arrowsDrawArray->dirty();
            arrowPoints->dirty();
            arrowColors->dirty();
            arrowGeometry->dirtyDisplayList();
            arrowGeometry->dirtyBound();
        } else {
            arrowsDrawArray->setCount(0);
            arrowsDrawArray->dirty();
            arrowPoints->dirty();
            arrowColors->dirty();
            arrowGeometry->dirtyDisplayList();
            arrowGeometry->dirtyBound();
        }

        //for (int i = 0; i < linesSum; i++) {
        //	int trailI = i + linesSum;
        //	if (lines[i].size() == 0 || lines[i].size() == 1) {
        //		(*linePoints)[trailI * 2].set(osg::Vec3(0.0, 0.0, 0.0));
        //		(*linePoints)[trailI * 2 + 1].set(osg::Vec3(0.0, 0.0, 0.0));
        //		(*lineColors)[trailI * 2].set(1.0, 1.0, 1.0, 1.0);
        //		(*lineColors)[trailI * 2 + 1].set(1.0, 1.0, 1.0, 1.0);

        //		continue;
        //	}
        //	int size = lines[i].size();
        //	int idx = (time_t-1) % size  <= 0 ? 1 : (time_t-1) % size;
        //	if (idx >= size || idx < 1) {
        //		std::cout << idx << " " << size << std::endl;
        //		idx = 1;
        //	}
        //	(*linePoints)[trailI * 2].set(lines[i][idx - 1]);
        //	(*linePoints)[trailI * 2 + 1].set(lines[i][idx]);

        //	osg::Vec3 c0 = colors[i][idx - 1];
        //	osg::Vec3 c1 = colors[i][idx];

        //	(*lineColors)[trailI * 2].set(c0[0], c0[1], c1[2], 0.8f);
        //	(*lineColors)[trailI * 2 + 1].set(c1[0], c1[1], c1[2], 0.8f);

        //	if (selected[i] == true) {
        //		(*lineColors)[trailI * 2].set(1.0f, 1.0f, 1.0f, 1.0f);
        //		(*lineColors)[trailI * 2 + 1].set(1.0f, 1.0f, 1.0f, 1.0f);
        //	}
        //}

        //for (int i = 0; i < linesSum; i++) {
        //	int trailI = i + linesSum*2;
        //	if (lines[i].size() == 0 || lines[i].size() == 1) {
        //		(*linePoints)[trailI * 2].set(osg::Vec3(0.0, 0.0, 0.0));
        //		(*linePoints)[trailI * 2 + 1].set(osg::Vec3(0.0, 0.0, 0.0));
        //		(*lineColors)[trailI * 2].set(1.0, 1.0, 1.0, 1.0);
        //		(*lineColors)[trailI * 2 + 1].set(1.0, 1.0, 1.0, 1.0);

        //		continue;
        //	}
        //	int size = lines[i].size();
        //	int idx = (time_t-2) % size <= 0 ? 1 : (time_t - 2) % size;
        //	if (idx >= size || idx < 1) {
        //		std::cout << idx << " " << size << std::endl;
        //		idx = 1;
        //	}
        //	(*linePoints)[trailI * 2].set(lines[i][idx - 1]);
        //	(*linePoints)[trailI * 2 + 1].set(lines[i][idx]);

        //	osg::Vec3 c0 = colors[i][idx - 1];
        //	osg::Vec3 c1 = colors[i][idx];

        //	(*lineColors)[trailI * 2].set(c0[0], c0[1], c1[2], 0.5f);
        //	(*lineColors)[trailI * 2 + 1].set(c1[0], c1[1], c1[2], 0.5f);

        //	if (selected[i] == true) {
        //		(*lineColors)[trailI * 2].set(1.0f, 1.0f, 1.0f, 1.0f);
        //		(*lineColors)[trailI * 2 + 1].set(1.0f, 1.0f, 1.0f, 1.0f);
        //	}
        //}

        linesDrawArray->setCount(linesSum * 2);
        linesDrawArray->dirty();
        linePoints->dirty();
        lineColors->dirty();
        geometry->dirtyDisplayList();
        geometry->dirtyBound();
    }

    osg::Vec3 StreamLineCPU::linearInterpolation(osg::Vec3 lonLatLev) {
        if (dataDim == EDataDimension::D2) {
            float x = lonLatLev.x();
            float y = lonLatLev.y();

            //float x = minX + rand() / (float)RAND_MAX * (maxX - minX);
            //float y = minY + rand() / (float)RAND_MAX * (maxY - minY);


            float i = (x - minX) / dx;
            float j = (y - minY) / dy;
            i = osg::clampBetween(i, 0.0f, dimX - 2);
            j = osg::clampBetween(j, 0.0f, dimY - 2);
            float intPart;
            float fractionI = modf(i, &intPart);
            float fractionJ = modf(j, &intPart);
            int intI = int(i);
            int intJ = int(j);


            int lb = intI * dimY + intJ;
            int rb = (intI + 1) * dimY + intJ;
            int lt = intI * dimY + intJ + 1;
            int rt = (intI + 1) * dimY + intJ + 1;


            float s, t;
            if (this->u[lb] == 9999 ||
                this->u[rb] == 9999 ||
                this->u[lt] == 9999 ||
                this->u[rt] == 9999)
                s = 0;
            else {
                s = this->u[lb] * (1 - fractionI) * (1 - fractionJ) +
                    this->u[rb] * fractionI * (1 - fractionJ) +
                    this->u[lt] * (1 - fractionI) * fractionJ +
                    this->u[rt] * fractionI * fractionJ;
            }
            if (this->v[lb] == 9999 ||
                this->v[rb] == 9999 ||
                this->v[lt] == 9999 ||
                this->v[rt] == 9999)
                t = 0;
            else {
                t = this->v[lb] * (1 - fractionI) * (1 - fractionJ) +
                    this->v[rb] * fractionI * (1 - fractionJ) +
                    this->v[lt] * (1 - fractionI) * fractionJ +
                    this->v[rt] * fractionI * fractionJ;
            }

            return osg::Vec3(s, t, 0.0);
        } else if (dataDim == EDataDimension::D3) {
            float x = lonLatLev.x();
            float y = lonLatLev.y();
            float z = lonLatLev.z();

            //float x = minX + rand() / (float)RAND_MAX * (maxX - minX);
            //float y = minY + rand() / (float)RAND_MAX * (maxY - minY);

            // reverse lon lat lev to data dimension index
            float i = (x - minX) / dx;
            float j = (y - minY) / dy;
            float k = (z - minZ) / dz;

            i = osg::clampBetween(i, 0.0f, dimX - 2);
            j = osg::clampBetween(j, 0.0f, dimY - 2);
            k = osg::clampBetween(k, 0.0f, dimZ - 2);
            float intPart;
            float fractionI = modf(i, &intPart);
            float fractionJ = modf(j, &intPart);
            float fractionK = modf(k, &intPart);
            int intI = int(i);
            int intJ = int(j);
            int intK = int(k);


            // int zOffset = dimX*dimY*intK;
            // // zOffset = dimX*dimY;
            // int lb = intI * dimY + intJ + zOffset;
            // int rb = (intI + 1) * dimY + intJ + zOffset;
            // int lt = intI * dimY + intJ + 1 + zOffset;
            // int rt = (intI + 1) * dimY + intJ + 1 + zOffset;
            //
            //
            // float s, t, p;
            // if (this->u[lb] == 9999 ||
            // 	this->u[rb] == 9999 ||
            // 	this->u[lt] == 9999 ||
            // 	this->u[rt] == 9999)
            // 	s = 0;
            // else {
            // 	s = this->u[lb] * (1 - fractionI) * (1 - fractionJ) +
            // 		this->u[rb] * fractionI * (1 - fractionJ) +
            // 		this->u[lt] * (1 - fractionI) * fractionJ +
            // 		this->u[rt] * fractionI * fractionJ;
            // }
            // if (this->v[lb] == 9999 ||
            // 	this->v[rb] == 9999 ||
            // 	this->v[lt] == 9999 ||
            // 	this->v[rt] == 9999)
            // 	t = 0;
            // else {
            // 	t = this->v[lb] * (1 - fractionI) * (1 - fractionJ) +
            // 		this->v[rb] * fractionI * (1 - fractionJ) +
            // 		this->v[lt] * (1 - fractionI) * fractionJ +
            // 		this->v[rt] * fractionI * fractionJ;
            // }
            //
            // p = k * 1.0f;
            //
            // return osg::Vec3(s, t, p);


            int c000 = intK * dimX * dimY + intI * dimY + intJ;
            int c001 = (intK + 1) * dimX * dimY + intI * dimY + intJ;
            int c010 = intK * dimX * dimY + intI * dimY + (intJ + 1);
            int c011 = (intK + 1) * dimX * dimY + intI * dimY + (intJ + 1);
            int c100 = intK * dimX * dimY + (intI + 1) * dimY + intJ;
            int c101 = (intK + 1) * dimX * dimY + (intI + 1) * dimY + intJ;
            int c110 = intK * dimX * dimY + (intI + 1) * dimY + (intJ + 1);
            int c111 = (intK + 1) * dimX * dimY + (intI + 1) * dimY + (intJ + 1);

            float s, t, wVal;
            // Interpolation for u component
            if (this->u[c000] == 9999 || this->u[c001] == 9999 ||
                this->u[c010] == 9999 || this->u[c011] == 9999 ||
                this->u[c100] == 9999 || this->u[c101] == 9999 ||
                this->u[c110] == 9999 || this->u[c111] == 9999) {
                s = 0;
            } else {
                s = this->u[c000] * (1 - fractionI) * (1 - fractionJ) * (1 - fractionK) +
                    this->u[c100] * fractionI * (1 - fractionJ) * (1 - fractionK) +
                    this->u[c010] * (1 - fractionI) * fractionJ * (1 - fractionK) +
                    this->u[c001] * (1 - fractionI) * (1 - fractionJ) * fractionK +
                    this->u[c101] * fractionI * (1 - fractionJ) * fractionK +
                    this->u[c011] * (1 - fractionI) * fractionJ * fractionK +
                    this->u[c110] * fractionI * fractionJ * (1 - fractionK) +
                    this->u[c111] * fractionI * fractionJ * fractionK;
            }

            // Interpolation for v component
            if (this->v[c000] == 9999 || this->v[c001] == 9999 ||
                this->v[c010] == 9999 || this->v[c011] == 9999 ||
                this->v[c100] == 9999 || this->v[c101] == 9999 ||
                this->v[c110] == 9999 || this->v[c111] == 9999) {
                t = 0;
            } else {
                t = this->v[c000] * (1 - fractionI) * (1 - fractionJ) * (1 - fractionK) +
                    this->v[c100] * fractionI * (1 - fractionJ) * (1 - fractionK) +
                    this->v[c010] * (1 - fractionI) * fractionJ * (1 - fractionK) +
                    this->v[c001] * (1 - fractionI) * (1 - fractionJ) * fractionK +
                    this->v[c101] * fractionI * (1 - fractionJ) * fractionK +
                    this->v[c011] * (1 - fractionI) * fractionJ * fractionK +
                    this->v[c110] * fractionI * fractionJ * (1 - fractionK) +
                    this->v[c111] * fractionI * fractionJ * fractionK;
            }

            // Interpolation for w component
            if (this->w[c000] == 9999 || this->w[c001] == 9999 ||
                this->w[c010] == 9999 || this->w[c011] == 9999 ||
                this->w[c100] == 9999 || this->w[c101] == 9999 ||
                this->w[c110] == 9999 || this->w[c111] == 9999) {
                wVal = 0;
            } else {
                wVal = this->w[c000] * (1 - fractionI) * (1 - fractionJ) * (1 - fractionK) +
                       this->w[c100] * fractionI * (1 - fractionJ) * (1 - fractionK) +
                       this->w[c010] * (1 - fractionI) * fractionJ * (1 - fractionK) +
                       this->w[c001] * (1 - fractionI) * (1 - fractionJ) * fractionK +
                       this->w[c101] * fractionI * (1 - fractionJ) * fractionK +
                       this->w[c011] * (1 - fractionI) * fractionJ * fractionK +
                       this->w[c110] * fractionI * fractionJ * (1 - fractionK) +
                       this->w[c111] * fractionI * fractionJ * fractionK;
            }

            return osg::Vec3(s, t, wVal * 100.0f);
        }
    }


    void StreamLineCPU::updateLineStyle(int value) {
        lineStyle = value;
        updateNodeGeometryCallbackPtr->updateLineStyle(lineStyle);
    }
}
