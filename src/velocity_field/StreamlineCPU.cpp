#include "StreamlineCPU.h"
#include "streamline_cpu.h"


namespace osg_3d_vis {
	StreamLineCPU::StreamLineCPU(osgViewer::Viewer& viewer, const osg::ref_ptr<osg::Group>& root, const osg::ref_ptr<osg::Camera>& mainCamera, osg_3d_vis::llhRange range) {
		return ;
		this->initFromDatFile(std::string(OSG_3D_VIS_DATA_PREFIX) + "U.DAT", std::string(OSG_3D_VIS_DATA_PREFIX) + "V.DAT", range);
		this->setRoot(root);
		this->setShaderPath(std::string(OSG_3D_VIS_SHADER_PREFIX));

		this->initializeDifferentialMethod();
		this->initializeSmoothAlgorithm();
		this->initializeRandomSeedSelection();
		this->initializeTexturesAndImages();
		this->initializeLineColorOptions();
		this->initializeSelectionRGBA();

		this->regenerateRandomPointsAndSteamLines();
		this->initializeLineGeometryRenderState();
		this->initializeArrowGeometryRenderState();

	
		/* get main camera color & depth buffer */
		mainCamera->attach(osg::Camera::COLOR_BUFFER, this->screenColorTexture);
		mainCamera->attach(osg::Camera::DEPTH_BUFFER, this->screenDepthTexture);

		/* multi-pass creation */
		osg::ref_ptr<osg::Camera> slaveCamera = this->createSegmentDrawPass(mainCamera);
		slaveCamera->setGraphicsContext(mainCamera->getGraphicsContext());
		viewer.addSlave(slaveCamera, false);
		//root->addChild(sl->createSegmentDrawPass(camera));
		root->addChild(this->createTrailDrawPass());
		root->addChild(this->createCopyPass());
		this->createScreenDrawPass();

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
		fs.read((char*)&temp, sizeof(float));
		fs.read((char*)_u, sizeof(float) * 52000);
		fs.close();

		fs.open(str2, std::ios::binary);
		if (!fs.is_open()) std::cout << "Cannot open file from path: " << "str2" << std::endl;
		fs.read((char*)&temp, sizeof(float));
		fs.read((char*)_v, sizeof(float) * 52000);
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
		pointDensity = 2.0f;
		h = 0.5f;
		speedScaleFactor = 0.5f;
		lineLength = 10;
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
		arrowPoints->resize(linesSum * 4, osg::Vec3(0.0f,  0.0, 0.0));
		arrowColors = new osg::Vec4Array();
		arrowColors->resize(linesSum*4);
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

		float* translucentColors = new float[sizeof(float) * 4 * this->rttTextureSize * this->rttTextureSize];
		for (int i = 0; i < this->rttTextureSize * this->rttTextureSize; i++) {
			translucentColors[i * 4 + 0] = 0.0f;
			translucentColors[i * 4 + 1] = 0.0f;
			translucentColors[i * 4 + 2] = 0.0f;
			translucentColors[i * 4 + 3] = 0.0f;
		}
		osg::ref_ptr<osg::Image> translucentImage = new osg::Image();
		translucentImage->setImage(rttTextureSize, rttTextureSize, 1, GL_RGBA, GL_RGBA, GL_FLOAT, (unsigned char*)translucentColors, osg::Image::USE_NEW_DELETE);

		nextTrailColorTexture->setImage(translucentImage);
		//currTrailColorTexture->setImage(translucentImage);

		screenDepthTexture = new osg::Texture2D;
		screenDepthTexture->setSourceFormat(GL_DEPTH_COMPONENT);
		screenDepthTexture->setSourceType(GL_FLOAT);
		screenDepthTexture->setInternalFormat(GL_DEPTH_COMPONENT32);
		screenDepthTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		screenDepthTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		screenDepthTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		screenDepthTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

		segmentDepthTexture = new osg::Texture2D;
		segmentDepthTexture->setSourceFormat(GL_DEPTH_COMPONENT);
		segmentDepthTexture->setSourceType(GL_FLOAT);
		segmentDepthTexture->setInternalFormat(GL_DEPTH_COMPONENT32);
		segmentDepthTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		segmentDepthTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		segmentDepthTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		segmentDepthTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

		currTrailDepthTexture = new osg::Texture2D;
		currTrailDepthTexture->setSourceFormat(GL_DEPTH_COMPONENT);
		currTrailDepthTexture->setSourceType(GL_FLOAT);
		currTrailDepthTexture->setInternalFormat(GL_DEPTH_COMPONENT32);
		currTrailDepthTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		currTrailDepthTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		currTrailDepthTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		currTrailDepthTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

		nextTrailDepthTexture = new osg::Texture2D;
		nextTrailDepthTexture->setSourceFormat(GL_DEPTH_COMPONENT);
		nextTrailDepthTexture->setSourceType(GL_FLOAT);
		nextTrailDepthTexture->setInternalFormat(GL_DEPTH_COMPONENT32);
		nextTrailDepthTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
		nextTrailDepthTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
		nextTrailDepthTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		nextTrailDepthTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

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
			switch (randomSeedSelectionAlgorithm) {
				case PSEUDO_RANDOM: {
					randX = minX + (maxX - minX) * randX;
					randY = minY + (maxY - minY) * randY;
					break;
				}
				case DENSITY_BASED: {
					float randTmp = distrib(gen) * (samplerLength - 1);
					float xStart = densityWeightedTile[(int)randTmp * 3];
					float yStart = densityWeightedTile[(int)randTmp * 3 + 1];
					randX = xStart + randX * tileDivideXInterval;
					randY = yStart + randY * tileDivideYInterval;
					randX = minX + (maxX - minX) * randX;
					randY = minY + (maxY - minY) * randY;
					break;
				}
				case INFORMATION_ENTROPY_BASED: {
					float randTmp = distrib(gen) * samplerLength;
					float xStart = infoEntropyWeightedTile[(int)randTmp * 3];
					float yStart = infoEntropyWeightedTile[(int)randTmp * 3 + 1];
					randX = xStart + randX * tileDivideXInterval;
					randY = yStart + randY * tileDivideYInterval;
					randX = minX + (maxX - minX) * randX;
					randY = minY + (maxY - minY) * randY;
					break;
				}
				default: {
					randX = minX + (maxX - minX) * randX;
					randY = minY + (maxY - minY) * randY;
					break;
				}
			}

			//std::cout << randX << ", " << randY << std::endl;
			makeStreamLine(i, randX, randY, geometry, linePoints, lineColors);
		}


		std::cout << "randomPoints generation completed!" << std::endl;
		updateGeometry(0);
	}
	osg::Vec2 StreamLineCPU::Rotate(osg::Vec2 v, float theta) {
		return osg::Vec2(v.x() * cos(theta) - v.y() * cos(theta), v.x() * sin(theta) + v.y() * cos(theta));
	}
	osg::Vec2 StreamLineCPU::constructCircularArc(float t, osg::Vec2 prevPosition, osg::Vec2 currentPosition, osg::Vec2 nextPosition) {
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
	osg::Vec2 StreamLineCPU::constructBezierCurve(float t, osg::Vec2 prevPosition, osg::Vec2 currentPosition, osg::Vec2 nextPosition) {
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

	void StreamLineCPU::processPosAndSpeed(int index, osg::Vec3 nextPos, osg::Vec3 speed) {
		double x, y, z;
		llh2xyz_Ellipsoid(osg::DegreesToRadians(nextPos.x()), osg::DegreesToRadians(nextPos.y() + 180.0), 1000, x, y, z);
		lines[index].push_back(osg::Vec3(x, y, z));
		/* transform  [lon&lat] --> [0, 1] */
		//lines[index].push_back(osg::Vec3(
		//	(nextPos.x() - minX)/(maxX - minX),
		//	(nextPos.y() - minY)/(maxY-minY),
		//	0.0));
		colors[index].push_back(osg::Vec4(hsvToRgb(osg::Vec3(getHueByVector(osg::Vec2(speed[0], speed[1]), hueRange, hueOffset), hsvSaturation, hsvValue)), 1.0f));
		speeds[index].push_back(speed);


		/* calculate arrows points */
		osg::Vec2 dir = osg::Vec2(speed[0], speed[1]);
		dir.normalize();
		osg::Vec2 lArrowDir = Rotate(-dir, osg::PI / 6.0) * 0.1f;
		osg::Vec2 rArrowDir = Rotate(-dir, -osg::PI / 6.0) * 0.1f;
		osg::Vec2 lArrow = osg::Vec2(nextPos.x() + lArrowDir.x(), nextPos.y() + lArrowDir.y());
		osg::Vec2 rArrow = osg::Vec2(nextPos.x() + rArrowDir.x(), nextPos.y() + rArrowDir.y());
		llh2xyz_Ellipsoid(osg::DegreesToRadians(lArrow.x()), osg::DegreesToRadians(lArrow.y() + 180.0), 1000, x, y, z);
		leftArrows[index].push_back(osg::Vec3(x, y, z));
		llh2xyz_Ellipsoid(osg::DegreesToRadians(rArrow.x()), osg::DegreesToRadians(rArrow.y() + 180.0), 1000, x, y, z);
		rightArrows[index].push_back(osg::Vec3(x, y, z));
	}

	void StreamLineCPU::regenerateLineColors() {
		for (int i = 0; i < speeds.size(); i++) {
			for (int j = 0; j < speeds[i].size(); j++) {
				osg::Vec3 tmpSpeed = speeds[i][j];
				colors[i][j] = osg::Vec4(osg::Vec4(hsvToRgb(osg::Vec3(getHueByVector(osg::Vec2(tmpSpeed[0], tmpSpeed[1]), hueRange, hueOffset), hsvSaturation, hsvValue)), 1.0f));
			}
		}
	}

	void StreamLineCPU::makeStreamLine(int index, float x0, float y0, osg::ref_ptr<osg::Geometry> geometry, osg::ref_ptr<osg::Vec3Array> linePoints, osg::ref_ptr<osg::Vec4Array> lineColors) {
		// Runge-Kutta Second-order methods with two stages 
		// aka midpoint method
		osg::Vec3 y_n = osg::Vec3(x0, y0, 0);
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

			if (this->smoothAlgorithm == STRAIGHT_LINE) 
			{
				processPosAndSpeed(index, nextPos, speed);
			}
			else if (this->smoothAlgorithm == CIRCULAR_ARC) 
			{
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
					}
					else if (samplePoint == 1.0f) {
						smoothedPoint = nextPos;
					}
					else {
						smoothedPoint = osg::Vec3(constructCircularArc(samplePoint,
							osg::Vec2(prevPos.x(), prevPos.y()), 
							osg::Vec2(currPos.x(), currPos.y()), 
							osg::Vec2(nextPos.x(), nextPos.y())
							),	0.0);
					}

					processPosAndSpeed(index, smoothedPoint, speed);


				}
			}
			else if (this->smoothAlgorithm == BEZIER_CURVE) 
			{
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
					}
					else if (samplePoint == 1.0f) {
						smoothedPoint = nextPos;
					}
					else {
						smoothedPoint = osg::Vec3(constructBezierCurve(samplePoint,
							osg::Vec2(prevPos.x(), prevPos.y()),
							osg::Vec2(currPos.x(), currPos.y()),
							osg::Vec2(nextPos.x(), nextPos.y())
						), 0.0);
					}

					processPosAndSpeed(index, smoothedPoint, speed);

				}
			}

			y_n = nextPos;

			/* minimum line length */
			tmpLength++;
			if (tmpLength < lineLength) continue;

			/* random decay */
			float randSeed = rand() / (float)RAND_MAX;
			if (randSeed < dropBase + dropBump * speed.x() + dropBase * speed.y()) return;

		}

	}

	void StreamLineCPU::updateGeometry(int time_t) {
		for (int i = 0; i < linesSum; i++) {
			if (lines[i].size() == 0 || lines[i].size() == 1) {
				(*linePoints)[i * 2].set(osg::Vec3(0.0, 0.0, 0.0));
				(*linePoints)[i * 2 + 1].set(osg::Vec3(0.0, 0.0, 0.0));
				(*lineColors)[i * 2].set(1.0, 1.0, 1.0, 1.0);
				(*lineColors)[i * 2 + 1].set(1.0, 1.0, 1.0, 1.0);

				continue;
			}
			int size = lines[i].size();
			int idx = time_t % size == 0 ? 1 : time_t % size;
			if (idx >= size || idx < 1) {
				std::cout << idx << " " << size << std::endl;
				idx = 1;
			}
			(*linePoints)[i * 2].set(lines[i][idx - 1]);
			(*linePoints)[i * 2 + 1].set(lines[i][idx]);

			osg::Vec4 c0 = colors[i][idx - 1];
			osg::Vec4 c1 = colors[i][idx];

			if (lineUseSameColor) {
				c0 = c1 = lineRGBA;
			}

			if (selected[i] == true) {
				c0 = c1 = selectionRGBA;
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
				}
				else {
					(*arrowColors)[i * 4 ].set(arrowRGBA[0], arrowRGBA[1], arrowRGBA[2], arrowRGBA[3]);
					(*arrowColors)[i * 4 + 1].set(arrowRGBA[0], arrowRGBA[1], arrowRGBA[2], arrowRGBA[3]);
					(*arrowColors)[i * 4 + 2].set(arrowRGBA[0], arrowRGBA[1], arrowRGBA[2], arrowRGBA[3]);
					(*arrowColors)[i * 4 + 3].set(arrowRGBA[0], arrowRGBA[1], arrowRGBA[2], arrowRGBA[3]);
				}
			}
		}

		if (showArrow) {
			arrowsDrawArray->setCount(linesSum * 4);
			arrowsDrawArray->dirty();
			arrowPoints->dirty();
			arrowColors->dirty();
			arrowGeometry->dirtyDisplayList();
			arrowGeometry->dirtyBound();
		}
		else {
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
	}




	osg::ref_ptr<osg::Camera> StreamLineCPU::createSegmentDrawPass(osg::ref_ptr<osg::Camera> mainCamera) {
		segmentDrawCamera = new osg::Camera;

		osg::ref_ptr<osg::StateSet> stateset = this->geometry->getOrCreateStateSet();
		//  Create vertex and fragment shaders
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
		vertexShader->loadShaderSourceFromFile(shaderPath + "segmentDraw.vs");
		fragmentShader->loadShaderSourceFromFile(shaderPath + "segmentDraw.fs");


		// Create a shader program and attach the shaders
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addBindAttribLocation("Vertex", 0);
		program->addBindAttribLocation("VertexColor", 1);
		program->addShader(vertexShader.get());
		program->addShader(fragmentShader.get());
		stateset->setAttributeAndModes(program);

		// �� mvp
		osg::Uniform* mvpUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "ModelViewProjectionMatrix");
		mvpUniform->setUpdateCallback(new MVPRedrawCallback(mainCamera, this));
		stateset->addUniform(mvpUniform);

		osg::ref_ptr<osg::Geode> tmpGeode = new osg::Geode;
		tmpGeode->addDrawable(this->geometry);
		updateNodeGeometryCallbackPtr = new updateNodeGeometryCallback(this);
		tmpGeode->addUpdateCallback(updateNodeGeometryCallbackPtr);

		//stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		//stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		//segmentDrawCamera->setStateSet(stateset);

		segmentDrawCamera->addChild(tmpGeode);

		segmentDrawCamera->setClearColor(osg::Vec4());
		segmentDrawCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//segmentDrawCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		segmentDrawCamera->setRenderOrder(osg::Camera::PRE_RENDER, 0);
		segmentDrawCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
		segmentDrawCamera->setViewport(0, 0, this->rttTextureSize, this->rttTextureSize);
		segmentDrawCamera->attach(osg::Camera::COLOR_BUFFER, segmentColorTexture);
		segmentDrawCamera->attach(osg::Camera::DEPTH_BUFFER, segmentDepthTexture);
		return segmentDrawCamera;
	}
	class updateTrailDrawPassCallback : public osg::Camera::DrawCallback {
	public:
		StreamLineCPU* sl;
		updateTrailDrawPassCallback(StreamLineCPU* _sl) {
			sl = _sl;
		}
		virtual void operator()(osg::RenderInfo& renderInfo) const
		{
			//osg::State* state = renderInfo.getState();
			//state->applyAttribute(sl->nextTrailColorTexture.get());
			//osg::Image* image = new osg::Image();
			//image->readImageFromCurrentTexture(renderInfo.getContextID(), false, GL_FLOAT);
			//osgDB::writeImageFile(*image, "snap.png");

			//osg::State* state = renderInfo.getState();
			//state->apply(renderInfo.getCurrentCamera()->getStateSet());
			//state->applyTextureAttribute(0, sl->currTrailColorTexture);
			//osg::Image* image = new osg::Image();
			//sl->currTrailColorTexture->apply(*state);
			//image->readImageFromCurrentTexture(renderInfo.getContextID(), true,
			//	GL_FLOAT); //change the GL_FLOAT to your needs
			//
			//osgDB::writeImageFile(*image, "snap.png");
			//sl->currTrailColorTexture->setImage(image);
			//sl->currTrailColorTexture->dirtyTextureObject();

			
			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (sl->trailDrawCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			stateset->getUniform("first")->set(bool(false));

		}
	};
	osg::ref_ptr<osg::Camera> StreamLineCPU::createTrailDrawPass() {
		trailDrawCamera = new osg::Camera;

		osg::ref_ptr<osg::Geometry> trailDrawPassGometry = createScreenQuad();
		osg::ref_ptr<osg::StateSet> stateset = trailDrawPassGometry->getOrCreateStateSet();
		//  Create vertex and fragment shaders
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
		vertexShader->loadShaderSourceFromFile(shaderPath + "fullscreenQuad.vs");
		fragmentShader->loadShaderSourceFromFile(shaderPath + "trailDraw.fs");


		// Create a shader program and attach the shaders
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addBindAttribLocation("Vertex", 0);
		program->addBindAttribLocation("TexCoord", 1);
		program->addShader(vertexShader.get());
		program->addShader(fragmentShader.get());
		stateset->setAttributeAndModes(program);


		osg::ref_ptr<osg::Uniform> uniformTrailColor = new osg::Uniform("trailColorTexture", 0);
		stateset->setTextureAttributeAndModes(0, currTrailColorTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformTrailColor);
		osg::ref_ptr<osg::Uniform> uniformTrailDepth = new osg::Uniform("trailDepthTexture", 1);
		stateset->setTextureAttributeAndModes(1, currTrailDepthTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformTrailDepth);
		osg::ref_ptr<osg::Uniform> uniformSegmentColor = new osg::Uniform("segmentColorTexture", 2);
		stateset->setTextureAttributeAndModes(2, segmentColorTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformSegmentColor);
		osg::ref_ptr<osg::Uniform> uniformSegmentDepth = new osg::Uniform("segmentDepthTexture", 3);
		stateset->setTextureAttributeAndModes(3, segmentDepthTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformSegmentDepth);

		osg::ref_ptr<osg::Uniform> firstUniform = new osg::Uniform("first", bool(true));
		stateset->addUniform(firstUniform);

		osg::ref_ptr<osg::Uniform> fadeOpacityUniform = new osg::Uniform("fadeOpacity", (float)0.95f);
		stateset->addUniform(fadeOpacityUniform);


		osg::ref_ptr<osg::Geode> tmpGeode = new osg::Geode;
		tmpGeode->addDrawable(trailDrawPassGometry);
		trailDrawCamera->addChild(tmpGeode);


		trailDrawCamera->setClearColor(osg::Vec4());
		trailDrawCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		trailDrawCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		trailDrawCamera->setRenderOrder(osg::Camera::PRE_RENDER, 1);
		trailDrawCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
		trailDrawCamera->setViewport(0, 0, this->rttTextureSize, this->rttTextureSize);
		trailDrawCamera->attach(osg::Camera::COLOR_BUFFER, nextTrailColorTexture);
		trailDrawCamera->attach(osg::Camera::DEPTH_BUFFER, nextTrailDepthTexture);

		trailDrawCamera->setPostDrawCallback(new updateTrailDrawPassCallback(this));

		return trailDrawCamera;
	}


	osg::ref_ptr<osg::Camera> StreamLineCPU::createScreenDrawPass() {
		osg::ref_ptr<osg::Camera> screenDrawCamera = new osg::Camera;

		osg::ref_ptr<osg::Geometry> renderPassGeometry = createScreenQuad();
		//osg::ref_ptr<osg::Geometry> renderPassGeometry = createScreenQuad();
		osg::ref_ptr<osg::StateSet> stateset = renderPassGeometry->getOrCreateStateSet();
		// Create vertex and fragment shaders
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
		vertexShader->loadShaderSourceFromFile(shaderPath + "fullscreenQuad.vs");
		fragmentShader->loadShaderSourceFromFile(shaderPath + "screenDraw.fs");


		// Create a shader program and attach the shaders
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addBindAttribLocation("Vertex", 0);
		program->addBindAttribLocation("TexCoord", 1);
		program->addShader(vertexShader.get());
		program->addShader(fragmentShader.get());
		stateset->setAttributeAndModes(program);


		osg::ref_ptr<osg::Uniform> uniformScreenColor = new osg::Uniform("screenColorTexture", 0);
		stateset->setTextureAttributeAndModes(0, screenColorTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformScreenColor);

		osg::ref_ptr<osg::Uniform> uniformScreenDepth = new osg::Uniform("screenDepthTexture", 1);
		stateset->setTextureAttributeAndModes(1, screenDepthTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformScreenDepth);

		osg::ref_ptr<osg::Uniform> uniformSegmentColor = new osg::Uniform("segmentColorTexture", 2);
		stateset->setTextureAttributeAndModes(2, currTrailColorTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformSegmentColor);

		osg::ref_ptr<osg::Uniform> uniformSegmentDepth = new osg::Uniform("segmentDepthTexture", 3);
		stateset->setTextureAttributeAndModes(3, currTrailDepthTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(uniformSegmentDepth);


		// Blend Rendering Related ʹ����ɫ��ALPHAͨ������͸��������Ⱦ
		stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
		stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		//stateset->setRenderBinDetails(11, "RenderBin");

		renderPassGeometry->setStateSet(stateset);


		osg::ref_ptr<osg::Geode> tmpGeode = new osg::Geode;
		tmpGeode->addDrawable(renderPassGeometry);
		osg::ref_ptr<osg::Transform> tmpNode = new osg::Transform;
		tmpNode->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		tmpNode->addChild(tmpGeode);

		root->addChild(tmpNode);

		//screenDrawCamera->addChild(tmpGeode);

		//screenDrawCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
		//screenDrawCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//screenDrawCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		//screenDrawCamera->setRenderOrder(osg::Camera::PRE_RENDER, 2);

		//screenDrawCamera->setViewport(0, 0, 1024, 768);

		return screenDrawCamera;
	}

	osg::ref_ptr<osg::Camera> StreamLineCPU::createCopyPass() {
		osg::ref_ptr<osg::Camera> copyCamera = new osg::Camera;
		osg::ref_ptr<osg::Geometry> copyPassGeometry = createScreenQuad();
		osg::ref_ptr<osg::StateSet> stateset = copyPassGeometry->getOrCreateStateSet();
		osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX); // Define your vertex shader source
		osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT); // Define your fragment shader source
		vertexShader->loadShaderSourceFromFile(shaderPath + "fullscreenQuad.vs");
		fragmentShader->loadShaderSourceFromFile(shaderPath + "copyColorAndDepth.fs");
		osg::ref_ptr<osg::Program> program = new osg::Program;
		program->addBindAttribLocation("Vertex", 0);
		program->addBindAttribLocation("TexCoord", 1);
		program->addShader(vertexShader.get());
		program->addShader(fragmentShader.get());
		stateset->setAttributeAndModes(program);

		osg::ref_ptr<osg::Uniform> nextTrailColorTextureUniform = new osg::Uniform("colorTexture", 0);
		stateset->setTextureAttributeAndModes(0, nextTrailColorTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(nextTrailColorTextureUniform);

		osg::ref_ptr<osg::Uniform> nextTrailDepthTextureUniform = new osg::Uniform("depthTexture", 1);
		stateset->setTextureAttributeAndModes(1, nextTrailDepthTexture.get(), osg::StateAttribute::ON);
		stateset->addUniform(nextTrailDepthTextureUniform);

		osg::ref_ptr<osg::Geode> tmpGeo = new osg::Geode;
		tmpGeo->addDrawable(copyPassGeometry.get());
		copyCamera->addChild(tmpGeo);

		copyCamera->setRenderOrder(osg::Camera::PRE_RENDER, 2);
		copyCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
		copyCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		copyCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		copyCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
		copyCamera->setViewport(0, 0, this->rttTextureSize, this->rttTextureSize);
		copyCamera->attach(osg::Camera::COLOR_BUFFER, currTrailColorTexture);
		copyCamera->attach(osg::Camera::DEPTH_BUFFER, currTrailDepthTexture);
		return copyCamera;
	}




	void StreamLineCPU::initializeTileWeightTexture() {

		samplerLength = 8192;
		tileDivideXSum = tileDivideYSum = 50;
		tileDivideXInterval = 1.0f / tileDivideXSum;
		tileDivideYInterval = 1.0f / tileDivideYSum;
		longitudeNum = 205;
		latitudeNum = 249;

		// calculate weightSum
		//float weightSum = tileDivideXSum* tileDivideYSum * 5.0f;
		double densityWeightSum = 0.0f;
		double infoEntropyWeightSum = 0.0f;
		std::vector<float> areaDensity;
		std::vector<float> areaInformationEntropy;
		areaDensity.resize(tileDivideXSum * tileDivideYSum);
		areaInformationEntropy.resize(tileDivideXSum * tileDivideYSum);

		for (int i = 0; i < tileDivideXSum; i++) {
			for (int j = 0; j < tileDivideYSum; j++) {
				float density = calculateAreaDensity(i, j);
				areaDensity[i * tileDivideYSum + j] = density;
				densityWeightSum += density;

				float infoEntropy = calculateAreaInformationEntropy(i, j);
				areaInformationEntropy[i * tileDivideYSum + j] = infoEntropy;
				infoEntropyWeightSum += infoEntropy;

			}
		}


		// transform xyWeight to 1DTexture
		float interval = samplerLength / densityWeightSum;
		float tmpPos = 0.0f;
		//float* densityWeightedTile = new float[sizeof(float) * 3 * samplerLength];
		densityWeightedTile.resize(3 * samplerLength);
		for (int i = 0; i < tileDivideXSum; i++) {
			for (int j = 0; j < tileDivideYSum; j++) {
				//float tileWeight = j < tileDivideYSum/2.0f ? 2.0f : 8.0f;
				float tileWeight = areaDensity[i * tileDivideYSum + j];
				float currentTileLength = tileWeight * interval;
				int start = int(tmpPos);
				int end = int(tmpPos + currentTileLength);
				for (int k = start; k < end && k < samplerLength; k++) {
					densityWeightedTile[k * 3 + 0] = float(i) / tileDivideXSum;
					densityWeightedTile[k * 3 + 1] = float(j) / tileDivideYSum;
					densityWeightedTile[k * 3 + 2] = 0.0f;
				}
				tmpPos += currentTileLength;
			}
		}
		//weightedTileImage = new osg::Image();
		//weightedTileImage->setImage(samplerLength, 1, 1, GL_RGB, GL_RGB, GL_FLOAT, (unsigned char*)densityWeightedTile, osg::Image::USE_NEW_DELETE);
		////osgDB::writeImageFile(*weightedTileImage, "weightedTileImage.png");
		//densityWeightedTileTex1D = new osg::Texture2D(weightedTileImage);
		//densityWeightedTileTex1D->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
		//densityWeightedTileTex1D->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);




		// initialize info Entropy Tex 1D
		interval = samplerLength / infoEntropyWeightSum;
		tmpPos = 0.0f;
		//float* infoEntropyWeightedTile = new float[sizeof(float) * 3 * samplerLength];
		infoEntropyWeightedTile.resize( 3 * samplerLength);
		for (int i = 0; i < tileDivideXSum; i++) {
			for (int j = 0; j < tileDivideYSum; j++) {
				//float tileWeight = j < tileDivideYSum/2.0f ? 2.0f : 8.0f;
				float tileWeight = areaInformationEntropy[i * tileDivideYSum + j];
				float currentTileLength = tileWeight * interval;
				int start = int(tmpPos);
				int end = int(tmpPos + currentTileLength);
				for (int k = start; k < end && k < samplerLength; k++) {
					infoEntropyWeightedTile[k * 3 + 0] = float(i) / tileDivideXSum;
					infoEntropyWeightedTile[k * 3 + 1] = float(j) / tileDivideYSum;
					infoEntropyWeightedTile[k * 3 + 2] = 0.0f;
				}
				tmpPos += currentTileLength;
			}
		}
		//osg::ref_ptr<osg::Image> infoEntropyWeightedTileImage = new osg::Image();
		//infoEntropyWeightedTileImage->setImage(samplerLength, 1, 1, GL_RGB, GL_RGB, GL_FLOAT, (unsigned char*)infoEntropyWeightedTile, osg::Image::USE_NEW_DELETE);
		//osgDB::writeImageFile(*weightedTileImage, "weightedTileImage.png");
		//informEntropyWeightedTileTex1D = new osg::Texture2D(infoEntropyWeightedTileImage);
		//informEntropyWeightedTileTex1D->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST);
		//informEntropyWeightedTileTex1D->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST);



	}

	/* 
	* 
	Information Entropy is Calculated by Single Vector's Dir Diff Between Area Vsectors' Sum(or Avg) Dir 
	Area Information Entropy is the Sum of the Information Entropy
	*
	*/

	float StreamLineCPU::calculateAreaInformationEntropy(int x, int y) {
		int lonStart = int(float(x) / tileDivideXSum * longitudeNum);
		int latStart = int(float(y) / tileDivideYSum * latitudeNum);
		int lonEnd = lonStart + int(tileDivideXInterval * longitudeNum);
		int latEnd = latStart + int(tileDivideYInterval * latitudeNum);
		int nSum = (lonEnd - lonStart + 1) * (latEnd - latStart + 1);
		float areaDensitySum = 0.0f;
		osg::Vec2 areaUVToward = osg::Vec2();
		for (int i = lonStart; i <= lonEnd; i++) {
			for (int j = latStart; j <= latEnd; j++) {
				float uVal = std::abs(u[i * latitudeNum + j]);
				float vVal = std::abs(v[i * latitudeNum + j]);
				if (uVal != 9999 && vVal != 9999) {
					//areaDensitySum += uVal + vVal;
					areaUVToward += osg::Vec2(uVal, vVal);
				}
			}
		}

		float areaDensityAvg = areaDensitySum / (float)nSum;
		areaUVToward.normalize();
		float areaInformationEntropy = 0.0f;
		for (int i = lonStart; i <= lonEnd; i++) {
			for (int j = latStart; j <= latEnd; j++) {
				float uVal = std::abs(u[i * latitudeNum + j]);
				float vVal = std::abs(v[i * latitudeNum + j]);
				if (uVal != 9999 && vVal != 9999) {
					osg::Vec2 dir(uVal, vVal);
					dir.normalize();
					areaInformationEntropy += osg::square(1.0 - dir * areaUVToward);
					//areaInformationEntropy += osg::square((uVal + vVal - areaDensityAvg));
				}
			}
		}
		return areaInformationEntropy;
	}
	float StreamLineCPU::calculateAreaDensity(int x, int y) {
		int lonStart = int(float(x) / tileDivideXSum * longitudeNum);
		int latStart = int(float(y) / tileDivideYSum * latitudeNum);
		int lonEnd = lonStart + int(tileDivideXInterval * longitudeNum);
		int latEnd = latStart + int(tileDivideYInterval * latitudeNum);
		float areaDensity = 0.0f;
		for (int i = lonStart; i <= lonEnd; i++) {
			for (int j = latStart; j <= latEnd; j++) {
				float uVal = std::abs(u[i * latitudeNum + j]);
				float vVal = std::abs(v[i * latitudeNum + j]);
				if (uVal != 9999 && vVal != 9999) {
						areaDensity += osg::square(uVal + vVal);
				}
			}
		}
		return areaDensity;
	}


	void StreamLineCPU::updateLineStyle(int value) {
		lineStyle = value;
		updateNodeGeometryCallbackPtr->updateLineStyle(lineStyle);
	}
}
