#ifndef SELECTION_H
#define SELECTION_H


#include <osg/Texture2D>
#include <osg/Group>
#include <osgDB/ReadFile> 
#include <osgViewer/Viewer>
#include <cmath>
#include <sstream>
#include <osgUtil/PolytopeIntersector>
#include <fstream>
#include <osg/LineWidth>
#include <osgDB/WriteFile>
#include <random>

#include "Util.h"

namespace Selection {
	class updateNodeGeometryCallback;

	inline 	osg::ref_ptr<osg::Geometry>createScreenQuad() {
		osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
		osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
		osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
		v->push_back(osg::Vec3(-1, -1, 0.0));
		v->push_back(osg::Vec3(1, -1, 0.0));
		v->push_back(osg::Vec3(1, 1, 0.0));
		v->push_back(osg::Vec3(-1, 1, 0.0));

		vt->push_back(osg::Vec2(0, 0));
		vt->push_back(osg::Vec2(1, 0));
		vt->push_back(osg::Vec2(1, 1));
		vt->push_back(osg::Vec2(0, 1));
		geometry->setVertexAttribArray(0, v.get(), osg::Array::BIND_PER_VERTEX);
		geometry->setVertexAttribArray(1, vt.get(), osg::Array::BIND_PER_VERTEX);
		osg::ref_ptr<osg::DrawArrays> quad = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4);
		geometry->addPrimitiveSet(quad.get());
		return geometry;
	}

	inline float getHueByVector(osg::Vec2 v, float hueRange, float hueOffset)
	{
		float hue = (atan2(v.y(), v.x()) / 3.1415926 + 1.0f) * 180;
		return std::fmodf(hue * hueRange / 360.0 + hueOffset, 360.0f);
	}
	inline osg::Vec3  hsvToRgb(osg::Vec3 hsv)
	{
		int H = (int(hsv.x())) % 360;
		float S = hsv.y();
		float V = hsv.z();
		int Hi = (H / 60) % 6;
		float f = (H / 60.0) - Hi;
		float p = V * (1 - S);
		float q = V * (1 - f * S);
		float t = V * (1 - (1 - f) * S);
		float R, G, B;
		switch (Hi) {
		case 0:
			R = V; G = t; B = p; break;
		case 1:
			R = q; G = V; B = p; break;
		case 2:
			R = p; G = V; B = t; break;
		case 3:
			R = p; G = q; B = V; break;
		case 4:
			R = t; G = p; B = V; break;
		case 5:
			R = V; G = p; B = q; break;
		}
		return osg::Vec3(R, G, B);
	}

	class StreamLine {
	public:
		StreamLine() {};
		StreamLine(osgViewer::Viewer& viewer, osg::ref_ptr<osg::Group> root, osg::ref_ptr<osg::Camera> mainCamera, llhRange range = llhRange());

		float dimX, dimY;
		float h;
		float dx, dy, dr;
		float minX, minY, maxX, maxY;
		float speedScaleFactor;
		float pointDensity;
		int idx;
		int lineLength;
		int pointsSum;
		int linesSum;
		std::vector<float> u, v;
		std::vector< std::vector<osg::Vec3> > lines;
		std::vector< std::vector<osg::Vec4> > colors;
		std::vector< std::vector<osg::Vec3> > speeds;
		std::vector< std::vector<osg::Vec3> > leftArrows;
		std::vector< std::vector<osg::Vec3> > rightArrows;

		osg::ref_ptr<osg::Geometry> geometry;
		osg::ref_ptr<osg::Vec3Array> linePoints;
		osg::ref_ptr<osg::Vec4Array> lineColors;
		osg::ref_ptr<osg::DrawArrays> linesDrawArray;

		osg::ref_ptr<osg::Geometry> arrowGeometry;
		osg::ref_ptr<osg::Vec3Array> arrowPoints;
		osg::ref_ptr<osg::Vec4Array> arrowColors;
		osg::ref_ptr<osg::DrawArrays> arrowsDrawArray;
		
		std::map<int, bool> selected;

		void initFromDatFile(std::string str1, std::string str2, llhRange range);
		void initializeTexturesAndImages();
		void initializeLineGeometryRenderState();
		void initializeArrowGeometryRenderState();
		void makeStreamLine(int index, float x0, float y0, osg::ref_ptr<osg::Geometry> geometry, osg::ref_ptr<osg::Vec3Array> linePoints, osg::ref_ptr<osg::Vec4Array> lineColors);
		void processPosAndSpeed(int index, osg::Vec3 nextPosition, osg::Vec3 speed);
		osg::Vec3 calculateSpeed(osg::Vec3 y_n);
		void regenerateRandomPointsAndSteamLines();
		void regenerateLineColors();
		void updateGeometry(int time_t);

		osg::Vec3 linearInterpolation(osg::Vec3 lonLatLev);
		osg::Vec2 Rotate(osg::Vec2 v, float theta);



		osg::ref_ptr<osg::Group> root;
		

		osg::ref_ptr<osg::Texture2D> screenColorTexture;
		osg::ref_ptr<osg::Texture2D> screenDepthTexture;
		osg::ref_ptr<osg::Texture2D> segmentColorTexture;
		osg::ref_ptr<osg::Texture2D> segmentDepthTexture;
		osg::ref_ptr<osg::Texture2D> currTrailColorTexture;
		osg::ref_ptr<osg::Texture2D> currTrailDepthTexture;
		osg::ref_ptr<osg::Texture2D> nextTrailColorTexture;
		osg::ref_ptr<osg::Texture2D> nextTrailDepthTexture;

		osg::ref_ptr<osg::Camera> trailDrawCamera;
		osg::ref_ptr<osg::Camera> segmentDrawCamera;

		osg::ref_ptr<osg::Camera> createSegmentDrawPass(osg::ref_ptr<osg::Camera> mainCamera);
		osg::ref_ptr<osg::Camera> createTrailDrawPass();
		osg::ref_ptr<osg::Camera> createScreenDrawPass();
		osg::ref_ptr<osg::Camera> createCopyPass();

		void updateTrailDrawPassTextures();



		int rttTextureSize;

		std::string shaderPath;

		osg::Matrixd mainCameraViewMatrix;

		/* simple get & set value funcs */ 
		bool isCameraSteady(osg::Matrixd _m) {
			return mainCameraViewMatrix == _m;
		}
		void updateMainCameraView(osg::Matrixd _m) {
			mainCameraViewMatrix = _m;
		}
		void setRoot(osg::ref_ptr<osg::Group> _root) {
			root = _root;
		}
		void setShaderPath(std::string _shaderPath) {
			shaderPath = _shaderPath;
		}


		/*
		* 
			below funcs are qt slots & its related funcs
		*
		*/

		/* lines attribute */
		void updateLineWidth(int value) {
			geometry->getOrCreateStateSet()->setAttributeAndModes(new osg::LineWidth(float(value) / 10.0f));
		}
		void updateFadeOpacity(int value) {
			osg::ref_ptr<osg::Geode> tmpGeo = dynamic_cast<osg::Geode*> (this->trailDrawCamera->getChild(0));
			osg::ref_ptr<osg::StateSet> stateset = tmpGeo->getDrawable(0)->getOrCreateStateSet();
			stateset->getUniform("fadeOpacity")->set((float)value / 100.0f );
		}
		void updateH(int value) {
			h = (float) value;
			this->regenerateRandomPointsAndSteamLines();
		}
		void updateSpeedScaleFactor(int value) {
			speedScaleFactor = (float) value / 50.0f;
			this->regenerateRandomPointsAndSteamLines();
		}
		void updatePointDensity(int value) {
			pointDensity = (float)value;
			this->regenerateRandomPointsAndSteamLines();
		}

		/* lines styles */
		int lineStyle;
		static constexpr int SOLID = 0;
		static constexpr int DOTTED = 1;
		updateNodeGeometryCallback* updateNodeGeometryCallbackPtr;
		void updateLineStyle(int value);

		/* lines colors*/
		bool lineUseSameColor;
		osg::Vec4 lineRGBA;
		float hueOffset;
		float hueRange;
		float hsvSaturation;
		float hsvValue;
		void initializeLineColorOptions() {
			lineUseSameColor = false;
			lineRGBA = osg::Vec4(1.0, 1.0, 1.0, 1.0);
			hueOffset = 0.0f;
			hueRange = 360.0f;
			hsvSaturation = 0.75f;
			hsvValue = 1.0f;
		}
		void updateLineColorTemplate(int value) {
			this->regenerateLineColors();
		}
		void updateHueOffset(int value) {
			hueOffset = value;
			this->regenerateLineColors();
		}
		void updateHueRange(int value) {
			hueRange = value;
			this->regenerateLineColors();
		}
		void updateSaturation(int value) {
			hsvSaturation = (float) value / 100.0f;
			this->regenerateLineColors();
		}
		void updateValue(int value) {
			hsvValue = (float) value / 100.0f;
			this->regenerateLineColors();
		}
		void updateLineUseSameColor(int value) {
			lineUseSameColor = value;
		}
		void updateLineR(int value) {
			this->lineRGBA.r() = (float)value / 255.0f;
		}
		void updateLineG(int value) {
			this->lineRGBA.g() = (float)value / 255.0f;
		}
		void updateLineB(int value) {
			this->lineRGBA.b() = (float)value / 255.0f;
		}
		void updateLineA(int value) {
			this->lineRGBA.a() = (float)value / 255.0f;
		}

		/* selection styles */
		osg::Vec4 selectionRGBA;
		void initializeSelectionRGBA() {
			selectionRGBA = osg::Vec4(1.0, 0.0, 0.0, 1.0);
		}
		void updateSelectionR(int value) {
			this->selectionRGBA.r() = (float)value / 255.0f;
		}
		void updateSelectionG(int value) {
			this->selectionRGBA.g() = (float)value / 255.0f;
		}
		void updateSelectionB(int value) {
			this->selectionRGBA.b() = (float)value / 255.0f;
		}
		void updateSelectionA(int value) {
			this->selectionRGBA.a() = (float)value / 255.0f;
		}

		/* arrows */
		bool showArrow;
		bool arrowUseLineColor;
		osg::Vec4 arrowRGBA;

		void updateArrowVisibility(int value) {
			this->showArrow = value;
		}
		void updateArrowUseLineColor(int value) {
			this->arrowUseLineColor = value;
		}
		void updateArrowR(int value) {
			this->arrowRGBA.r() = (float) value / 255.0f;
		}
		void updateArrowG(int value) {
			this->arrowRGBA.g() = (float)value / 255.0f;
		}
		void updateArrowB(int value) {
			this->arrowRGBA.b() = (float)value / 255.0f;
		}
		void updateArrowA(int value) {
			this->arrowRGBA.a() = (float)value / 255.0f;
		}


		/* random seed selection */
		int samplerLength;
		int tileDivideXSum, tileDivideYSum;
		float tileDivideXInterval, tileDivideYInterval;
		int longitudeNum, latitudeNum;
		std::vector<float> densityWeightedTile;
		std::vector<float> infoEntropyWeightedTile;
		int randomSeedSelectionAlgorithm;
		static constexpr int PSEUDO_RANDOM = 0;
		static constexpr int DENSITY_BASED = 1;
		static constexpr int INFORMATION_ENTROPY_BASED = 2;
		void initializeRandomSeedSelection() {
			randomSeedSelectionAlgorithm = PSEUDO_RANDOM;
		}
		void updateRandomSeedSelection(int value) {
			randomSeedSelectionAlgorithm = value;
			this->regenerateRandomPointsAndSteamLines();
		}

		/* smooth algorithm */
		int smoothAlgorithm;
		int lineSegmentSubdivision;
		float lineSegmentSubStep;
		static constexpr int STRAIGHT_LINE = 0;
		static constexpr int CIRCULAR_ARC = 1;
		static constexpr int BEZIER_CURVE = 2;
		void initializeTileWeightTexture();
		float calculateAreaInformationEntropy(int x, int y);
		float calculateAreaDensity(int x, int y);
		osg::Vec2 StreamLine::constructCircularArc(float t, osg::Vec2 prevPosition, osg::Vec2 currentPosition, osg::Vec2 nextPosition);
		osg::Vec2 StreamLine::constructBezierCurve(float t, osg::Vec2 prevPosition, osg::Vec2 currentPosition, osg::Vec2 nextPosition);
		void initializeSmoothAlgorithm() {
			smoothAlgorithm = STRAIGHT_LINE;
			lineSegmentSubdivision = 5;
			lineSegmentSubStep = 1.0f / lineSegmentSubdivision;
			initializeTileWeightTexture();
		}
		void updateSmoothAlgorithm(int value) {
			if (smoothAlgorithm == STRAIGHT_LINE 
				&& (value == CIRCULAR_ARC || value == BEZIER_CURVE) ) {
				h *= lineSegmentSubdivision;
			}
			if (value == STRAIGHT_LINE
				&& (smoothAlgorithm == CIRCULAR_ARC || smoothAlgorithm == BEZIER_CURVE)) {
				h /= lineSegmentSubdivision;
			}
			smoothAlgorithm = value;
			this->regenerateRandomPointsAndSteamLines();

		}


		/* differential method*/
		int differentialMethod;
		static constexpr int FORWARD_EULER = 0;
		static constexpr int RUNGE_KUTTA = 1;
		void initializeDifferentialMethod() {
			differentialMethod = FORWARD_EULER;
		}
		void updateDifferentialMethod(int value) {
			differentialMethod = value;
			this->regenerateRandomPointsAndSteamLines();
		}



	};

	// line selection 
	class PickHandler : public osgGA::GUIEventHandler {
	public:
		StreamLine* sl;
		PickHandler(StreamLine* _sl) : sl(_sl) {}

		~PickHandler() {}

		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

		virtual void pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea);

		void setLabel(const std::string& name)
		{
			//if (_updateText.get()) _updateText->setText(name);
		}

	protected:

	};

	inline bool PickHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		switch (ea.getEventType())
		{
		case(osgGA::GUIEventAdapter::PUSH):
		{
			osgViewer::View* view = dynamic_cast<osgViewer::Viewer*>(&aa);
			if (view) pick(view, ea);
			return false;
		}
		case(osgGA::GUIEventAdapter::KEYDOWN):
		{
			if (ea.getKey() == 'c')
			{
				//osgViewer::View* view = dynamic_cast<osgViewer::Viewer*>(&aa);
				//osg::ref_ptr<osgGA::GUIEventAdapter> event = new osgGA::GUIEventAdapter(ea);
				//event->setX((ea.getXmin() + ea.getXmax()) * 0.5);
				//event->setY((ea.getYmin() + ea.getYmax()) * 0.5);
				//if (view) pick(view, *event);
				sl->selected.clear();
			}
			return false;
		}
		default:
			return false;
		}
	}

	inline void PickHandler::pick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea)
	{
		osgUtil::LineSegmentIntersector::Intersections intersections;

		double mx = ea.getXnormalized();
		double my = ea.getYnormalized();
		osg::ref_ptr<osgUtil::PolytopeIntersector> picker =
			new osgUtil::PolytopeIntersector(osgUtil::Intersector::PROJECTION, mx - 0.02, my - 0.02, mx + 0.02, my + 0.02);
		picker->setDimensionMask(osgUtil::PolytopeIntersector::DimZero | osgUtil::PolytopeIntersector::DimOne);
		osgUtil::IntersectionVisitor iv(picker);
		sl->segmentDrawCamera->accept(iv);

		//BoundingBoxDisableNodeVisitor boundingBoxDisableVisitor = BoundingBoxDisableNodeVisitor();
		//m_ptrSceneNode->accept(boundingBoxDisableVisitor);


		if (picker->containsIntersections())
		{
			osgUtil::PolytopeIntersector::Intersections  intersections = picker->getIntersections();

			std::cout << "Intersected! ";
			for (osgUtil::PolytopeIntersector::Intersections::iterator hitr = intersections.begin();
				hitr != intersections.end();
				++hitr)
			{

				std::cout << hitr->primitiveIndex << std::endl;

				sl->selected[hitr->primitiveIndex] = true;

				//if (hitr->nodePath.empty()) {
				//	std::cout << "nodePath empty" << std::endl;
				//}

				//if (!hitr->nodePath.empty() && !(hitr->nodePath.back()->getName().empty()))
				//{
				//	// the geodes are identified by name.
				//	std::cout << "Object \"" << hitr->nodePath.back()->getName() << "\"" << std::endl;
				//}
				//else if (hitr->drawable.valid())
				//{
				//	std::cout << "Object \"" << hitr->drawable->className() << "\"" << std::endl;
				//}

			}
		}
	}

	class updateNodeGeometryCallback : public osg::NodeCallback {
	public:

		Selection::StreamLine* sl;
		long long int time_t;
		long long int animation_t;
		int updateFrameInterval;
		int lineStyle;
		static constexpr int SOLID = 0;
		static constexpr int DOTTED = 1;

		updateNodeGeometryCallback(Selection::StreamLine* _sl) : sl(_sl) {
			time_t = 0;
			animation_t = 0;
			updateFrameInterval = 3;
			lineStyle = SOLID;
		}

		virtual void operator ()(osg::Node* node, osg::NodeVisitor* nv) {
			time_t++;
			if (time_t % updateFrameInterval == 0) animation_t++;
			/* line style setup*/
			if (lineStyle == DOTTED) {
				/* jump points */
				for (int i = 2; i < 10; i++) {
					if (time_t % updateFrameInterval * i == 0) animation_t++;
				}
			}

			//std::cout << animation_t << std::endl;
			sl->updateGeometry(animation_t);
			traverse(node, nv);
		}
		void updateUpdateFrameInterval(int value) {
			updateFrameInterval = value;
		}
		void updateLineStyle(int value) {
			lineStyle = value;
		}

	};

	class MVPRedrawCallback : public osg::Uniform::Callback {
	public:
		osg::Camera* camera;
		Selection::StreamLine* sl;
		MVPRedrawCallback(osg::Camera* _camera, Selection::StreamLine* _sl) :
			camera(_camera), sl(_sl) {
		}

		virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv) {
			osg::Matrixd viewMatrix = camera->getViewMatrix();
			osg::Matrixd modelMatrix = osg::computeLocalToWorld(nv->getNodePath());
			osg::Matrixd modelViewProjectionMatrix = modelMatrix * viewMatrix * camera->getProjectionMatrix();
			uniform->set(modelViewProjectionMatrix);


			// dynamic redraw for non pixel texturing
			if (!sl->isCameraSteady(viewMatrix)) {
				osg::ref_ptr<osg::Image> subImage = sl->nextTrailColorTexture->getImage();
				osg::ref_ptr<osg::Image> tmpImage = new osg::Image;
				subImage->copySubImage(0, 0, 0, tmpImage);
				subImage->dirty();

				sl->updateMainCameraView(viewMatrix);
			}
		}
	};

	

}


#endif 