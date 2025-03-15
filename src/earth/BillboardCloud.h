#pragma once

#include <osg/StateAttribute>
#include <osg/BlendEquation>
#include <osg/MatrixTransform> 
#include <osg/BlendFunc>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Matrix>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Transform>
#include <osg/Material>
#include <osg/Depth>
#include <osg/CullFace>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/TextureCubeMap>
#include <osg/VertexProgram>

#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <osgUtil/SmoothingVisitor>
#include <osgUtil/Optimizer>

#include <osgViewer/Viewer>

#include <iostream>

#include "../util.h"

inline std::string CLOUD_FILENAME(const std::string& type, const std::string& name)
{
	return std::string(OSG_3D_VIS_DATA_PREFIX)+"/clouds/"+type+"/"+name+"-"+type+"/"+name+"-"+type+".obj";
}
inline std::string CUBEMAP_FILENAME(const std::string& face)
{
	return std::string(OSG_3D_VIS_DATA_PREFIX)+"/skybox/"+face+".png";
}
inline float RAND(float min, float max)
{
	return ((min) + (float)rand()/(RAND_MAX+1) * ((max)-(min)));
}

// SkyBox
osg::TextureCubeMap* readCubeMap()
{
    osg::TextureCubeMap* cubemap = new osg::TextureCubeMap;

    osg::ref_ptr<osg::Image>imagePosX = osgDB::readRefImageFile(CUBEMAP_FILENAME("posx"));
    osg::ref_ptr<osg::Image>imageNegX = osgDB::readRefImageFile(CUBEMAP_FILENAME("negx"));
    osg::ref_ptr<osg::Image>imagePosY = osgDB::readRefImageFile(CUBEMAP_FILENAME("posy"));
    osg::ref_ptr<osg::Image>imageNegY = osgDB::readRefImageFile(CUBEMAP_FILENAME("negy"));
    osg::ref_ptr<osg::Image>imagePosZ = osgDB::readRefImageFile(CUBEMAP_FILENAME("posz"));
    osg::ref_ptr<osg::Image>imageNegZ = osgDB::readRefImageFile(CUBEMAP_FILENAME("negz"));

    if (imagePosX && imageNegX && imagePosY && imageNegY && imagePosZ && imageNegZ)
    {
        cubemap->setImage(osg::TextureCubeMap::POSITIVE_X, imagePosX);
        cubemap->setImage(osg::TextureCubeMap::NEGATIVE_X, imageNegX);
        cubemap->setImage(osg::TextureCubeMap::POSITIVE_Y, imagePosY);
        cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Y, imageNegY);
        cubemap->setImage(osg::TextureCubeMap::POSITIVE_Z, imagePosZ);
        cubemap->setImage(osg::TextureCubeMap::NEGATIVE_Z, imageNegZ);

        cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
        cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
        cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

        cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
        cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    }

    return cubemap;
}
struct TexMatCallback : public osg::NodeCallback
{
	// Update texture matrix for cubemaps
public:

    TexMatCallback(osg::TexMat& tm) :
        _texMat(tm)
    {
    }

    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if (cv)
        {
            const osg::Matrix& MV = *(cv->getModelViewMatrix());
            const osg::Matrix R = osg::Matrix::rotate(osg::DegreesToRadians(112.0f), 0.0f, 0.0f, 1.0f) *
                osg::Matrix::rotate(osg::DegreesToRadians(90.0f), 1.0f, 0.0f, 0.0f);

            osg::Quat q = MV.getRotate();
            const osg::Matrix C = osg::Matrix::rotate(q.inverse());

            _texMat.setMatrix(C * R);
        }

        traverse(node, nv);
    }

    osg::TexMat& _texMat;
};
class MoveEarthySkyWithEyePointTransform : public osg::Transform
{
public:
    /** Get the transformation matrix which moves from local coords to world coords.*/
    virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const
    {
        osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if (cv)
        {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
            matrix.preMultTranslate(eyePointLocal);
        }
        return true;
    }

    /** Get the transformation matrix which moves from world coords to local coords.*/
    virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const
    {
        osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
        if (cv)
        {
            osg::Vec3 eyePointLocal = cv->getEyeLocal();
            matrix.postMultTranslate(-eyePointLocal);
        }
        return true;
    }
};
osg::Node* createSkyBox()
{

    osg::StateSet* stateset = new osg::StateSet();

    osg::TexEnv* te = new osg::TexEnv;
    te->setMode(osg::TexEnv::REPLACE);
    stateset->setTextureAttributeAndModes(0, te, osg::StateAttribute::ON);

    osg::TexGen* tg = new osg::TexGen;
    tg->setMode(osg::TexGen::NORMAL_MAP);
    stateset->setTextureAttributeAndModes(0, tg, osg::StateAttribute::ON);

    osg::TexMat* tm = new osg::TexMat;
    stateset->setTextureAttribute(0, tm);

    osg::TextureCubeMap* skymap = readCubeMap();
    stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);

    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    stateset->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);

    // clear the depth to the far plane.
    osg::Depth* depth = new osg::Depth;
    depth->setFunction(osg::Depth::ALWAYS);
    depth->setRange(1.0, 1.0);
    stateset->setAttributeAndModes(depth, osg::StateAttribute::ON);

    stateset->setRenderBinDetails(-1, "RenderBin");

    osg::Drawable* drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f, 0.0f, 0.0f), osg::WGS_84_RADIUS_EQUATOR*2));

    osg::Geode* geode = new osg::Geode;
    geode->setCullingActive(false);
    geode->setStateSet(stateset);
    geode->addDrawable(drawable);


    osg::Transform* transform = new MoveEarthySkyWithEyePointTransform;
    transform->setCullingActive(false);
    transform->addChild(geode);

    osg::ClearNode* clearNode = new osg::ClearNode;
    //  clearNode->setRequiresClear(false);
    clearNode->setCullCallback(new TexMatCallback(*tm));
    clearNode->addChild(transform);

    return clearNode;
}

osg::Group* CreateBaseIntersectSideCloud(osg::ref_ptr<osg::Node> cloudNode)
{
	osg::Group* baseCloud = new osg::Group;

	// 移动中心点到(0,0,0)， 并旋转至面对屏幕
	osg::Vec3 center = cloudNode->getBound().center();
	float radius = cloudNode->getBound().radius();
	osg::Matrix transM = osg::Matrix::translate(-center);
	osg::Matrix rotM = osg::Matrix::rotate(osg::DegreesToRadians(-90.0), osg::X_AXIS) * osg::Matrix::rotate(osg::DegreesToRadians(-90.0), osg::Z_AXIS);
	osg::ref_ptr<osg::MatrixTransform> originCloud = new osg::MatrixTransform;
	originCloud->setMatrix(transM * rotM);
	originCloud->addChild(cloudNode);

	// 绕z轴旋转90°， 作为旋转插片
	rotM = osg::Matrix::rotate(osg::DegreesToRadians(90.0), osg::Z_AXIS);
	osg::ref_ptr<osg::MatrixTransform> rotZCloud = new osg::MatrixTransform;
	rotZCloud->setMatrix(rotM);
	rotZCloud->addChild(originCloud);

	// 绕x轴旋转90°，往z轴负方向平移 作为旋转插片
	rotM = osg::Matrix::rotate(osg::DegreesToRadians(-90.0), osg::X_AXIS);
	transM = osg::Matrix::translate(osg::Vec3(0, 0, -radius*0.2));
	osg::ref_ptr<osg::MatrixTransform> rotXCloud = new osg::MatrixTransform;
	rotXCloud->setMatrix(rotM*transM);
	rotXCloud->addChild(originCloud);

	osg::Vec3 axis = osg::Vec3(1,1,0.5); axis.normalize();
	rotM = osg::Matrix::rotate(osg::X_AXIS, axis);
	transM = osg::Matrix::translate(osg::Vec3(0, 0, 0));
	osg::ref_ptr<osg::MatrixTransform> rotAxisCloud = new osg::MatrixTransform;
	rotAxisCloud->setMatrix(rotM*transM);
	rotAxisCloud->addChild(originCloud);

	baseCloud->addChild(originCloud);
	baseCloud->addChild(rotZCloud);
	baseCloud->addChild(rotXCloud);
	baseCloud->addChild(rotAxisCloud);

	return baseCloud;
}
osg::MatrixTransform* CreateBaseSideCloud(osg::ref_ptr<osg::Node> cloudNode)
{
	// 移动中心点到(0,0,0)， 并旋转至面对屏幕
	osg::Vec3 center = cloudNode->getBound().center();
	float radius = cloudNode->getBound().radius();
	osg::Matrix transM = osg::Matrix::translate(-center);
	osg::Matrix rotM = osg::Matrix::rotate(osg::DegreesToRadians(-90.0), osg::X_AXIS) * osg::Matrix::rotate(osg::DegreesToRadians(-90.0), osg::Z_AXIS);
	osg::ref_ptr<osg::MatrixTransform> baseCloud = new osg::MatrixTransform;
	baseCloud->setMatrix(transM * rotM);
	baseCloud->addChild(cloudNode);
	return baseCloud.release();
}
osg::MatrixTransform* CreateBaseTopCloud(osg::ref_ptr<osg::Node> cloudNode)
{
	// 移动中心点到(0,0,0)， 并旋转至XOY平面平行
	osg::Vec3 center = cloudNode->getBound().center();
	float radius = cloudNode->getBound().radius();
	osg::Matrix transM = osg::Matrix::translate(-center);
	osg::Matrix rotM = osg::Matrix::rotate(osg::DegreesToRadians(-90.0), osg::X_AXIS) * osg::Matrix::rotate(osg::DegreesToRadians(90.0), osg::Z_AXIS);

	osg::MatrixTransform* baseCloud = new osg::MatrixTransform;

	baseCloud->setMatrix(transM * rotM);
	baseCloud->addChild(cloudNode);
	return baseCloud;
}
osg::MatrixTransform* CreateTransformedCloud(osg::ref_ptr<osg::MatrixTransform> baseCloud, const osg::Vec3 &location)
{
	osg::MatrixTransform* local2WorldMT = new osg::MatrixTransform;
	osg::Matrix rotM = osg::Matrix::rotate(osg::DegreesToRadians(RAND(0, 180)), osg::Z_AXIS) * osg::Matrix::rotate(osg::Z_AXIS, location);
	osg::Matrix transM = osg::Matrix::translate(location);
	osg::Matrix scaleM = osg::Matrix::scale(osg::Vec3(100,100,100));
	local2WorldMT->setMatrix(scaleM * rotM * transM);
	local2WorldMT->addChild(baseCloud);
	return local2WorldMT;
}
osg::ref_ptr<osg::Vec3Array> generateRandomPointsOnSphere(int numPoints, float radius) {
    osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;
    for (int i = 0; i < numPoints; ++i) {
    	// 随机生成球面坐标
		double theta = acos(1.0 - 2.0 * static_cast<double>(rand()) / static_cast<double>(RAND_MAX)); // 极角
		double phi = 2.0 * osg::PI * static_cast<double>(rand()) / static_cast<double>(RAND_MAX);        // 方位角
        // 转换为笛卡尔坐标
        float x = radius * sin(theta) * cos(phi);
        float y = radius * sin(theta) * sin(phi);
        float z = radius * cos(theta);
		points->push_back(osg::Vec3(x, y, z));
    }
    return points;
}

class SpaceCell: public osg::Referenced {
public:
    SpaceCell(const osg::BoundingBox& bounds) : _bounds(bounds), _cloud(nullptr) {}

    osg::BoundingBox _bounds;
    osg::ref_ptr<osg::Group> _cloud;
};
class CloudSpaceManager : public osg::Referenced {
	public:
		CloudSpaceManager(float cellSize = 20000.0f) : _cellSize(cellSize),
			_centerX(0),
			_centerY(0),
			_centerZ(0)
		{
			_root = new osg::Group;
			// 设置雾效	
			osg::ref_ptr<osg::Fog> fog = new osg::Fog;
			{
				//fog->setColor(osg::Vec4(0.608, 0.737, 0.863, 0.0));
				//fog->setColor(osg::Vec4(0.17254901960784313, 0.36470588235294116, 0.6274509803921569, 0.0));
				fog->setColor(osg::Vec4(0.2727450980392157, 0.4845098039215686, 0.7550980392156863, 0.0));
				fog->setStart(0.01);
				fog->setEnd(cellSize * _cellRadius * 0.95);
				fog->setDensity(0.005);
				fog->setMode(osg::Fog::LINEAR);
			}
			_root->getOrCreateStateSet()->setAttributeAndModes(fog.get(), osg::StateAttribute::ON || osg::StateAttribute::OVERRIDE);
			// 读取文件
			initCloudNode();
			// 初始化创建空间网格
			for (int x = -_cellRadius; x <= _cellRadius; x++) {
				for (int y = -_cellRadius; y <= _cellRadius; y++) {
					for (int z = -_cellRadius; z <= _cellRadius; z++) {
						createCell(x, y, z);
					}
				}
			}
		}
		void createCell(int x, int y, int z) {
			osg::BoundingBox bounds(
				x * _cellSize,
				y * _cellSize,
				z * _cellSize,
				(x + 1) * _cellSize,
				(y + 1) * _cellSize,
				(z + 1) * _cellSize
			);
			// 将边界框的中心移动到网格中心
			osg::Vec3 offset(-_cellSize / 2, -_cellSize / 2, -_cellSize / 2);
			bounds._min += offset;
			bounds._max += offset;
	
			osg::ref_ptr<SpaceCell> cell = new SpaceCell(bounds);
			cell->_cloud = CreateCloudSceneRandomOnEarth();
			_cells.push_back(cell);
	
			// 调整云层位置到当前空间单元
			osg::Vec3 cellCenter(cell->_bounds.center().x(), cell->_bounds.center().y(), cell->_bounds.center().z());
			osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
			transform->setMatrix(osg::Matrix::translate(cellCenter));
			transform->addChild(cell->_cloud);
			_root->addChild(transform);
		}
		void update(const osg::Vec3& eyePos) {
			// 计算当前所在的网格坐标
			int gridX = static_cast<int>(floor((eyePos.x() + _cellSize / 2.f) / _cellSize));
			int gridY = static_cast<int>(floor((eyePos.y() + _cellSize / 2.f) / _cellSize));
			int gridZ = static_cast<int>(floor((eyePos.z() + _cellSize / 2.f) / _cellSize));
			// 检查是否需要添加网格
			if (needRebuildGrid(gridX, gridY, gridZ)) {
				rebuildGrid(gridX, gridY, gridZ);
	
				osg::MatrixTransform* mt = dynamic_cast<osg::MatrixTransform*>(_root->getParent(0)->getParent(0));
				osg::Vec3 direction = eyePos;
				direction.normalize();
				osg::Vec3 zAxis(0.0f, 0.0f, 1.0f);
				osg::Quat rotation;
				rotation.makeRotate(osg::Z_AXIS, direction);
				mt->setMatrix(osg::Matrix::translate(-eyePos) * osg::Matrix::rotate(rotation) * osg::Matrix::translate(eyePos));
			}
		}
		osg::Group* getRoot() { return _root.get(); }
	private:
		void initCloudNode()
		{
			_sideCloud04 = osgDB::readNodeFile(CLOUD_FILENAME("side", "cloud4"));
			_sideCloud05 = osgDB::readNodeFile(CLOUD_FILENAME("side", "cloud5"));
			_sideCloud06 = osgDB::readNodeFile(CLOUD_FILENAME("side", "cloud6"));
			_sideCloud07 = osgDB::readNodeFile(CLOUD_FILENAME("side", "cloud7"));
	
			_topCloud08 = osgDB::readNodeFile(CLOUD_FILENAME("top", "cloud8"));
			_topCloud09 = osgDB::readNodeFile(CLOUD_FILENAME("top", "cloud9"));
			_topCloud10 = osgDB::readNodeFile(CLOUD_FILENAME("top", "cloud10"));
			_topCloud11 = osgDB::readNodeFile(CLOUD_FILENAME("top", "cloud11"));
	
			// Set State
			osg::StateSet* dstate = new osg::StateSet;
			{
				osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
				blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				dstate->setAttributeAndModes(blendFunc);
				dstate->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
				dstate->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
				dstate->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
				dstate->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	
				_sideCloud04->setStateSet(dstate);
				_sideCloud05->setStateSet(dstate);
				_sideCloud06->setStateSet(dstate);
				_sideCloud07->setStateSet(dstate);
	
				_topCloud08->setStateSet(dstate);
				_topCloud09->setStateSet(dstate);
				_topCloud10->setStateSet(dstate);
				_topCloud11->setStateSet(dstate);
			}
	
			_baseSideCloud04 = CreateBaseSideCloud(_sideCloud04);
			_baseSideCloud05 = CreateBaseSideCloud(_sideCloud05);
			_baseSideCloud06 = CreateBaseSideCloud(_sideCloud06);
			_baseSideCloud07 = CreateBaseSideCloud(_sideCloud07);
			_baseTopCloud08 = CreateBaseTopCloud(_topCloud08);
			_baseTopCloud09 = CreateBaseTopCloud(_topCloud09);
			_baseTopCloud10 = CreateBaseTopCloud(_topCloud10);
			_baseTopCloud11 = CreateBaseTopCloud(_topCloud11);
		}
		osg::Group* CreateCloudSceneRandom()
		{
			osg::ref_ptr<osg::Group> root = new osg::Group;
			osg::ref_ptr<osg::Group> clouds = new osg::Group;
	
			osg::ref_ptr<osg::Group> sideClouds = new osg::Group;
			int nums = 20;
			while (nums--)
			{
				int type = rand() % 4;
				osg::Matrix rotM, transM;
				transM = osg::Matrix::translate(osg::Vec3(RAND(-_cellSize / 2.f, _cellSize / 2.f), RAND(3000.f, _cellSize), RAND(-_cellSize * 0.1f, 4000.f)));
				rotM = osg::Matrix::rotate(osg::DegreesToRadians(RAND(-180.f, 180.f)), osg::Z_AXIS);
				osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
				mt->setMatrix(transM * rotM);
	
				switch (type)
				{
				case 0:
					mt->addChild(_baseSideCloud04);
					break;
				case 1:
					mt->addChild(_baseSideCloud06);
					break;
				case 2:
					mt->addChild(_baseSideCloud06);
					break;
				case 3:
					mt->addChild(_baseSideCloud07);
					break;
				default:
					break;
				}
				sideClouds->addChild(mt);
			}
	
			osg::ref_ptr<osg::Group> topClouds = new osg::Group;
			{
				if (RAND(0.f, 1.f) < 0.5f)
				{
					osg::Matrix rotM, transM;
					transM = osg::Matrix::translate(osg::Vec3(RAND(-15000.f, 15000.f), 0.f, 6000.f));
					rotM = osg::Matrix::rotate(osg::DegreesToRadians(RAND(-180.f, 180.f)), osg::Z_AXIS);
					osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
					mt->setMatrix(transM * rotM);
					mt->addChild(_baseTopCloud08);
					topClouds->addChild(mt);
				}
				if (RAND(0.f, 1.f) < 0.3f)
				{
					osg::Matrix rotM, transM;
					transM = osg::Matrix::translate(osg::Vec3(RAND(-15000.f, 15000.f), 0.f, 6000.f));
					rotM = osg::Matrix::rotate(osg::DegreesToRadians(RAND(-180.f, 180.f)), osg::Z_AXIS);
					osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
					mt->setMatrix(transM * rotM);
					mt->addChild(_baseTopCloud09);
					topClouds->addChild(mt);
				}
				if (RAND(0.f, 1.f) < 0.3f)
				{
					osg::Matrix rotM, transM;
					transM = osg::Matrix::translate(osg::Vec3(RAND(-15000.f, 15000.f), 0.f, 6000.f));
					rotM = osg::Matrix::rotate(osg::DegreesToRadians(RAND(-180.f, 180.f)), osg::Z_AXIS);
					osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
					mt->setMatrix(transM * rotM);
					mt->addChild(_baseTopCloud10);
					topClouds->addChild(mt);
				}
				if (RAND(0.f, 1.f) < 0.3f)
				{
					osg::Matrix rotM, transM;
					transM = osg::Matrix::translate(osg::Vec3(RAND(-15000.f, 15000.f), 0.f, 6000.f));
					rotM = osg::Matrix::rotate(osg::DegreesToRadians(RAND(-180.f, 180.f)), osg::Z_AXIS);
					osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
					mt->setMatrix(transM * rotM);
					mt->addChild(_baseTopCloud11);
					topClouds->addChild(mt);
				}
			}
	
			clouds->addChild(sideClouds);
			clouds->addChild(topClouds);
	
			root->addChild(clouds);
			return root.release();
		}
		osg::Group* CreateCloudSceneRandomOnEarth()
		{
			osg::ref_ptr<osg::Group> root = new osg::Group;
			osg::ref_ptr<osg::Group> clouds = new osg::Group;
	
			osg::ref_ptr<osg::Group> sideClouds = new osg::Group;
			int nums = 10;
			while (nums--)
			{
				int type = rand() % 4;
				osg::Matrix rotM, transM;
				transM = osg::Matrix::translate(osg::Vec3(RAND(-_cellSize / 2.f, _cellSize / 2.f), RAND(-_cellSize / 2.f, _cellSize / 2.f), RAND(-_cellSize/2.f, _cellSize / 2.f)));
				rotM = osg::Matrix::rotate(osg::DegreesToRadians(RAND(-180.f, 180.f)), osg::Z_AXIS);
				osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
				mt->setMatrix(rotM*transM);
	
				switch (type)
				{
				case 0:
					mt->addChild(_baseSideCloud04);
					break;
				case 1:
					mt->addChild(_baseSideCloud06);
					break;
				case 2:
					mt->addChild(_baseSideCloud06);
					break;
				case 3:
					mt->addChild(_baseSideCloud07);
					break;
				default:
					break;
				}
				sideClouds->addChild(mt);
			}
			clouds->addChild(sideClouds);
			root->addChild(clouds);
			return root.release();
		}
		bool needRebuildGrid(int gridX, int gridY, int gridZ) {
			return abs(gridY - _centerY) >= _cellRadius ||
				abs(gridX - _centerX) >= _cellRadius || abs(gridZ - _centerZ) >= _cellRadius;
		}
		void rebuildGrid(int gridX, int gridY, int gridZ) {
			// 更新中心网格坐标
			_centerX = gridX;
			_centerY = gridY;
			_centerZ = gridZ;
	
			// 清除现有的云层
			_root->removeChildren(0, _root->getNumChildren());
			_cells.clear();
	
			// 以新的中心位置重建网格
			for (int x = -_cellRadius; x <= _cellRadius; x++) {
				for (int y = -_cellRadius; y <= _cellRadius; y++) {
					for (int z = -_cellRadius; z <= _cellRadius; z++) {
						createCell(x + gridX, y + gridY, z + gridZ);
					}
				}
			}
	
		}
	private:
		std::vector<osg::ref_ptr<SpaceCell>> _cells;
		osg::ref_ptr<osg::Group> _root;
		float _cellSize;
		int _centerX, _centerY, _centerZ;  // 当前网格中心坐标
		int _cellRadius = 1;
		int _cellNums = 2 * _cellRadius + 1;
	
		osg::ref_ptr<osg::Node> _sideCloud04;
		osg::ref_ptr<osg::Node> _sideCloud05;
		osg::ref_ptr<osg::Node> _sideCloud06;
		osg::ref_ptr<osg::Node> _sideCloud07;
	
		osg::ref_ptr<osg::Node> _topCloud08;
		osg::ref_ptr<osg::Node> _topCloud09;
		osg::ref_ptr<osg::Node> _topCloud10;
		osg::ref_ptr<osg::Node> _topCloud11;
	
		osg::ref_ptr<osg::MatrixTransform> _baseSideCloud04;
		osg::ref_ptr<osg::MatrixTransform> _baseSideCloud05;
		osg::ref_ptr<osg::MatrixTransform> _baseSideCloud06;
		osg::ref_ptr<osg::MatrixTransform> _baseSideCloud07;
	
		osg::ref_ptr<osg::MatrixTransform> _baseTopCloud08;
		osg::ref_ptr<osg::MatrixTransform> _baseTopCloud09;
		osg::ref_ptr<osg::MatrixTransform> _baseTopCloud10;
		osg::ref_ptr<osg::MatrixTransform> _baseTopCloud11;
	};
class CloudSceneFirstPersonManipulator : public osgGA::FirstPersonManipulator {
public:
	CloudSceneFirstPersonManipulator()
		: osgGA::FirstPersonManipulator() {
	}
protected:
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us)
	{
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::FRAME:
		{
			return handleFrame(ea, us);
		}

		case osgGA::GUIEventAdapter::RESIZE:
			return handleResize(ea, us);

		default:
			break;
		}
		if (ea.getHandled())
			return false;
		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::MOVE:
			return handleMouseMove(ea, us);

		case osgGA::GUIEventAdapter::DRAG:
			return handleMouseDrag(ea, us);

		case osgGA::GUIEventAdapter::PUSH:
			return handleMousePush(ea, us);

		case osgGA::GUIEventAdapter::RELEASE:
			return handleMouseRelease(ea, us);

		case osgGA::GUIEventAdapter::KEYDOWN:
			return handleKeyDown(ea, us);

		case osgGA::GUIEventAdapter::KEYUP:

			return handleKeyUp(ea, us);

		case osgGA::GUIEventAdapter::SCROLL:
			if (_flags & PROCESS_MOUSE_WHEEL)
				return handleMouseWheel(ea, us);
			else
				return false;

		default:
			return false;
		}
	}
	bool handleKeyDown(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) override {

		switch (ea.getKey())
		{
		case osgGA::GUIEventAdapter::KEY_Space:
			flushMouseEventStack();
			_thrown = false;
			home(ea, us);
			return true;
		case osgGA::GUIEventAdapter::KEY_W:
			flushMouseEventStack();
			_thrown = false;
			moveForward(_distance);
			us.requestRedraw();
			us.requestContinuousUpdate(isAnimating() || _thrown);
			return true;
		case osgGA::GUIEventAdapter::KEY_S:
			flushMouseEventStack();
			_thrown = false;
			moveForward(-_distance);
			us.requestRedraw();
			us.requestContinuousUpdate(isAnimating() || _thrown);
			return true;

		case osgGA::GUIEventAdapter::KEY_A:
			flushMouseEventStack();
			_thrown = false;
			moveRight(-_distance);
			us.requestRedraw();
			us.requestContinuousUpdate(isAnimating() || _thrown);
			return true;

		case osgGA::GUIEventAdapter::KEY_D:
			flushMouseEventStack();
			_thrown = false;
			moveRight(_distance);
			us.requestRedraw();
			us.requestContinuousUpdate(isAnimating() || _thrown);
			return true;
		case osgGA::GUIEventAdapter::KEY_Q:
			_distance -= 1000.0;
			_distance = std::max(100.0, _distance);
			return true;
		case osgGA::GUIEventAdapter::KEY_E:
			_distance += 1000.0;
			_distance = std::min(10000.0, _distance);
			return true;

		default:
			break;
		}

		return false;
	}
	void moveForward(const double distance)
	{
		moveForward(_rotation, distance);
	}
	void moveForward(const osg::Quat& rotation, const double distance)
	{
		osg::Vec3d forward = rotation * osg::Vec3d(0., 0., -distance);
		//forward.z() = 0;
		_eye += forward;
	}
	void moveRight(const double distance)
	{
		osg::Vec3d right = _rotation * osg::Vec3d(distance, 0., 0.);
		//right.z() = 0;
		_eye += right;
	}
public:
	double _distance = 3000.0;
};
class CloudUpdateCallback : public osg::NodeCallback {
public:
	CloudUpdateCallback(CloudSpaceManager* cloudManager, osgViewer::Viewer* viewer) :
		_cloudManager(cloudManager), _viewer(viewer) {
	}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
	{
		// 获取 CameraManipulator
		osgGA::CameraManipulator* manipulator = _viewer->getCameraManipulator();
		if (manipulator) {
			// 获取相机的世界坐标系矩阵（视图矩阵的逆矩阵）
			osg::Matrixd matrix = manipulator->getMatrix();
			osg::Vec3d cameraPosition = matrix.getTrans();
			
			_cloudManager->update(cameraPosition);
		}
		traverse(node, nv);
	}
	osg::ref_ptr<CloudSpaceManager> _cloudManager;
	osgViewer::Viewer* _viewer;
};
class VisibilityCallback :public osg::NodeCallback {
	public:
		VisibilityCallback(float distance=20000.f):_distance(distance){}
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) override
		{
			osg::Switch* switchNode = dynamic_cast<osg::Switch*>(node);
			if (!switchNode) return;
			// 获取CullVisitor以访问相机信息
			osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
			if (cv)
			{
				// 获取相机位置
				osg::Vec3 eyePoint = cv->getEyePoint();
	
				// 计算与原点的距离
				float d = eyePoint.length()-osg::WGS_84_RADIUS_EQUATOR;
	
				// 根据距离切换显示的节点
				if (d <= 0 || d > _distance)
				{
					switchNode->setChildValue(switchNode->getChild(0), false);
				}
				else
				{
					switchNode->setChildValue(switchNode->getChild(0), true);
				}
			
			}
	
			// 继续遍历
			traverse(node, nv);
	
		}
	
	private:
		float _distance;
	};
osg::ref_ptr<osg::Program> createCloudShaderProgram() {
    osg::ref_ptr<osg::Program> program = new osg::Program;

    osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
    vertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX)+"Cloud/shader.vert");
    program->addShader(vertexShader.get());
   
	osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
    fragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX)+"Cloud/shader.frag");
    program->addShader(fragmentShader.get());
    return program;
}
class ResolutionCallback : public osg::Uniform::Callback {
	public:
		ResolutionCallback(osg::Camera* camera) : _camera(camera) {}
		virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv) {
			osg::Viewport* vp = _camera->getViewport();
			uniform->set(osg::Vec2(vp->width(), vp->height()));
		}
	private:
		osg::Camera* _camera;
	};
class CloudRenderStateSetter : public osg::NodeVisitor {
public:
	CloudRenderStateSetter(osg::Camera* camera) :
			osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
			_camera(camera),
			_program(createCloudShaderProgram()){ }

	void apply(osg::MatrixTransform& mt) override {
		osg::StateSet* stateSet = mt.getOrCreateStateSet();
		osg::ref_ptr<osg::Uniform> modelViewProjectionUniform = new osg::Uniform(osg::Uniform::FLOAT_MAT4, "ModelViewProjectionMatrix");
		modelViewProjectionUniform->setUpdateCallback(new osg_3d_vis::ModelViewProjectionMatrixCallback(_camera));
		stateSet->addUniform(modelViewProjectionUniform.get());
		traverse(mt);
	}
	void apply(osg::Geode& geode) override {
		for (unsigned int i = 0; i < geode.getNumDrawables(); ++i) {
			osg::StateSet* stateSet = geode.getDrawable(i)->getOrCreateStateSet();
			osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
			// Not Working Correctly
			//geometry->setVertexAttribArray(0, geometry->getVertexArray(), osg::Array::BIND_OVERALL);
			//geometry->setVertexAttribArray(1, geometry->getNormalArray(), osg::Array::BIND_PER_VERTEX);
			//geometry->setVertexAttribArray(2, geometry->getTexCoordArray(0), osg::Array::BIND_PER_VERTEX);
			if (stateSet) {
				stateSet->setAttributeAndModes(_program.get(), osg::StateAttribute::ON);
				osg::ref_ptr<osg::Uniform> textureDiffuseUniform = new osg::Uniform("TextureDiffuse", 0);
				stateSet->addUniform(textureDiffuseUniform.get());
				osg::ref_ptr<osg::Uniform> resolutionUniform = new osg::Uniform("uResolution", osg::Vec2(0.f,0.f));
				resolutionUniform->setUpdateCallback(new ResolutionCallback(_camera));
				stateSet->addUniform(resolutionUniform.get());
				osg::ref_ptr<osg::Uniform> frameTimeUniform = new osg::Uniform("FrameTime", 0.0f);
				frameTimeUniform->setUpdateCallback(new osg_3d_vis::TimerCallback());
				stateSet->addUniform(frameTimeUniform.get());
			}
		}
		traverse(geode);
	}
		osg::Camera* _camera;
		osg::ref_ptr<osg::Program> _program;
};

void applyCloudScene(osg::Group* root, osgViewer::Viewer* viewer)
{
	// Set Camera Manipulation
	osg::Vec3 eye(0, -osg::WGS_84_RADIUS_POLAR - 60000, 0);
	osg::Vec3 center(0.0f, 0.0f, 0.0f);
	osg::Vec3 up(0.0f, 0.0f, 1.0f);
	osg::ref_ptr<CloudSpaceManager> cloudManager = new CloudSpaceManager(15000.0f);
	osg::ref_ptr<CloudSceneFirstPersonManipulator> manipulator = new CloudSceneFirstPersonManipulator();
	viewer->setCameraManipulator(manipulator.get());
	manipulator->setHomePosition(eye, center, up);

	// SkyBox
	root->addChild(createSkyBox());

	// Side Clouds
	osg::ref_ptr<osg::Group> sideClouds = new osg::Group;
	sideClouds->addChild(cloudManager->getRoot());
	sideClouds->addUpdateCallback(new CloudUpdateCallback(cloudManager, viewer));
	osg::ref_ptr<osg::MatrixTransform> cloudsMT = new osg::MatrixTransform;
	cloudsMT->addChild(sideClouds);

	// Top Clouds
	osg::ref_ptr<osg::Node> topCloud08 = osgDB::readNodeFile(CLOUD_FILENAME("top", "cloud8"));
	osg::ref_ptr<osg::Node> topCloud09 = osgDB::readNodeFile(CLOUD_FILENAME("top", "cloud9"));
	osg::ref_ptr<osg::Node> topCloud11 = osgDB::readNodeFile(CLOUD_FILENAME("top", "cloud11"));
	// Set StateSet
	osg::StateSet* dstate = new osg::StateSet;
	{
		osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
		blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		dstate->setAttributeAndModes(blendFunc, osg::StateAttribute::ON);

		osg::ref_ptr<osg::Depth> depth = new osg::Depth();
		depth->setWriteMask(false);
		dstate->setAttributeAndModes(depth, osg::StateAttribute::ON);

		dstate->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		dstate->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
		dstate->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		topCloud08->setStateSet(dstate);
		topCloud09->setStateSet(dstate);
		topCloud11->setStateSet(dstate);
	}

	osg::ref_ptr<osg::Vec3Array> points = generateRandomPointsOnSphere(300, osg::WGS_84_RADIUS_POLAR + 50000);
	CloudRenderStateSetter setter(viewer->getCamera());

	osg::ref_ptr<osg::Group> topClouds = new osg::Group;
	for (const osg::Vec3& point : *points) {
		int type = rand() % 3;
		osg::ref_ptr<osg::Node> cloud;
		switch (type)
		{
		case 0:
			cloud = CreateTransformedCloud(CreateBaseTopCloud(topCloud08), point);
			break;
		case 1:
			cloud = CreateTransformedCloud(CreateBaseTopCloud(topCloud09), point);
			break;
		case 2:
			cloud = CreateTransformedCloud(CreateBaseTopCloud(topCloud11), point);
			break;
		default:
			break;
		}
		cloud->accept(setter);
		topClouds->addChild(cloud);
	}

	// Cloud Switcher
	osg::ref_ptr<osg::Switch> cloudSwitchNode = new osg::Switch;
	cloudSwitchNode->addChild(cloudsMT, false);
	cloudSwitchNode->addChild(topClouds, true);
	cloudSwitchNode->addCullCallback(new VisibilityCallback(25000.0f));
	root->addChild(cloudSwitchNode);
}