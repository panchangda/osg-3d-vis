#pragma once
#include <osgViewer/Viewer>
#include  <iostream>

#include "osg/Material"

const std::string OSG_3D_VIS_SHADER_PREFIX="/data/osg-3d-vis/shader/";
const std::string  OSG_3D_VIS_DATA_PREFIX="/data/osg-3d-vis/data/";
const std::string OSG_3D_VIS_CONFIG_PREFIX="/data/osg-3d-vis/config/";

namespace osg_3d_vis {

	struct PbrMaterial
	{
		osg::Vec4 Ao, ka, kd, ks;
		float shiness;
		osg::Vec3 F0{ 0.562,0.565,0.544 };
	};
	struct GeometryData {
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;  // Vertex positions as Vec3Array
		osg::ref_ptr<osg::Vec2Array> uvs = new osg::Vec2Array;       // Texture coordinates as Vec2Array
		osg::ref_ptr<osg::Vec3Array> noramls = new osg::Vec3Array;
		std::vector<osg::ref_ptr<osg::Texture>> textures;            // Textures
		osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES); // Index array
		osg::ref_ptr<osg::DrawElementsUShort> shortIndices = new osg::DrawElementsUShort(GL_TRIANGLES);
		PbrMaterial materials;           
	};

	class GeometryExtractor : public osg::NodeVisitor {
	public:
		std::vector<GeometryData> geometries;

		GeometryExtractor() : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

		virtual void apply(osg::Geode& geode) {
			for (unsigned int i = 0; i < geode.getNumDrawables(); ++i) {
				osg::ref_ptr<osg::Geometry> geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(i));
				if (geometry) {
					GeometryData data;

					// Get vertex array
					osg::ref_ptr<osg::Vec3Array> vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
					if (vertices) {
						data.vertices->insert(data.vertices->end(), vertices->begin(), vertices->end());
					}

					// Get texture coordinate array (only handling the first UV set)
					if (geometry->getNumTexCoordArrays() > 0) {
						osg::ref_ptr<osg::Vec2Array> uvs = dynamic_cast<osg::Vec2Array*>(geometry->getTexCoordArray(0));
						if (uvs) {
							data.uvs->insert(data.uvs->end(), uvs->begin(), uvs->end());
						}
					}
					osg::ref_ptr<osg::Vec3Array> normals = dynamic_cast<osg::Vec3Array*>(geometry->getNormalArray());
					if (normals) {
						data.noramls->insert(data.noramls->end(), normals->begin(), normals->end());
					}

					// Extract index array from each primitive set
					for (unsigned int p = 0; p < geometry->getNumPrimitiveSets(); ++p) {
						osg::PrimitiveSet* primitiveSet = geometry->getPrimitiveSet(p);
						osg::DrawElementsUInt* drawElements = dynamic_cast<osg::DrawElementsUInt*>(primitiveSet);
						auto drawIndices = dynamic_cast<osg::DrawElementsUShort*>(primitiveSet);
						if (drawElements) {
							data.indices->insert(data.indices->end(), drawElements->begin(), drawElements->end());
						}
						if( drawIndices){
							data.shortIndices->insert(data.shortIndices->end(), drawIndices->begin(), drawIndices->end());
						}
					}

					// Check for and extract textures on all texture units
					osg::StateSet* stateSet = geometry->getStateSet();
					if (stateSet) {
						for (unsigned int unit = 0; unit < 16; ++unit) { // Assuming up to 16 texture units
							osg::Texture* texture = dynamic_cast<osg::Texture*>(stateSet->getTextureAttribute(unit, osg::StateAttribute::TEXTURE));
							if (texture) {
								data.textures.push_back(texture);
							}
						}

					}

					// 将数据存入 geometries 向量
					geometries.push_back(data);
				}
			}
			traverse(geode);
		}
	};


struct llhRange{
	double minLatitude;
	double maxLatitude;
	double minLongtitude;
	double maxLongtitude;
	double minHeight;
	double maxHeight;
	llhRange() {
		minLatitude = osg::DegreesToRadians(-90.0);
		maxLatitude = osg::DegreesToRadians(+90.0);
		minLongtitude = osg::DegreesToRadians(-180.0);
		maxLongtitude = osg::DegreesToRadians(+180.0);
		minHeight = 0.0;
		maxHeight = 100000.0;
	}
	llhRange(double _minLatitude, double _maxLatitude,
		double _minLongitute, double _maxLongitute,
		double _minHeight = 0, double _maxHeight = 0)
	:minLatitude(osg::DegreesToRadians(_minLatitude)), maxLatitude(osg::DegreesToRadians(_maxLatitude)),
		minLongtitude(osg::DegreesToRadians(_minLongitute+180)), maxLongtitude(osg::DegreesToRadians(_maxLongitute+180)),
		minHeight(_minHeight), maxHeight(_maxHeight){}

};
inline void llh2xyz_Sphere(llhRange llh,
	float _lat, float _lon, float _h, float& x, float& y, float& z) {
	double hDlt = llh.maxHeight - llh.minHeight;
	double latDlt = llh.maxLatitude - llh.minLatitude;
	double lonDlt = llh.maxLongtitude - llh.minLongtitude;

	double lat = llh.minLatitude + latDlt * _lat;
	double lon = llh.minLongtitude + lonDlt * _lon;
	double h = llh.minHeight + hDlt * _h + osg::WGS_84_RADIUS_EQUATOR;

	z = h * sin(lat);
	float r = h * cos(lat);
	x = r * cos(lon);
	y = r * sin(lon);
}

// transform to llhRange
inline void llh2xyz_Ellipsoid(llhRange llh,
	double _lat, double _lon, double _h, double& x, double& y, double& z) {
	double hDlt = llh.maxHeight - llh.minHeight;
	double latDlt = llh.maxLatitude - llh.minLatitude;
	double lonDlt = llh.maxLongtitude - llh.minLongtitude;

	double lat = llh.minLatitude + latDlt * _lat;
	double lon = llh.minLongtitude + lonDlt * _lon;
	double h = llh.minHeight + hDlt * _h;

	auto pEllModel = new osg::EllipsoidModel();
	pEllModel->convertLatLongHeightToXYZ(lat, lon, h, x, y, z);
}

inline void llh2xyz_Ellipsoid(double _lat, double _lon, double _h, double &x, double &y, double &z) {
	double lat = _lat;
	double lon = _lon;
	double h = _h;
	static osg::EllipsoidModel* pEllModel = new osg::EllipsoidModel();
	pEllModel->convertLatLongHeightToXYZ(lat, lon, h, x, y, z);
}

inline void llh2xyz_Ellipsoid(osg::Vec3 v, double& x, double& y, double& z) {

	double lat = v.x();
	double lon = v.y();
	double h = v.z();
	static osg::EllipsoidModel* pEllModel = new osg::EllipsoidModel();
	pEllModel->convertLatLongHeightToXYZ(lat, lon, h, x, y, z);
}


inline osg::Vec3d llh2xyz_Ellipsoid(double _lat, double _lon, double _h) {
	double lat = _lat;
	double lon = _lon;
	double h = _h;
	auto pEllModel = new osg::EllipsoidModel();
	osg::Vec3d xyz(0.0f, 0.0f, 0.0f);
	pEllModel->convertLatLongHeightToXYZ(lat, lon, h, xyz.x(), xyz.y(), xyz.z());
	return xyz;
}

class TimeUniformCallback : public osg::Uniform::Callback {
public:
	virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv) {
		// 逐帧更新的逻辑，例如，更新时间值
		static float time = 0.0f;
		time += 0.001f; // 每帧增加 0.01，调整以适应应用需求

		// 将更新后的值设置到 uniform
		uniform->set(time);
	}
};

struct ModelViewProjectionMatrixCallback : public osg::Uniform::Callback
{
	ModelViewProjectionMatrixCallback(osg::Camera* camera) :
		_camera(camera) {
	}

	virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* nv) {
		osg::Matrixd viewMatrix = _camera->getViewMatrix();
		osg::Matrixd modelMatrix = osg::computeLocalToWorld(nv->getNodePath());
		osg::Matrixd modelViewProjectionMatrix = modelMatrix * viewMatrix * _camera->getProjectionMatrix();
		uniform->set(modelViewProjectionMatrix);
	}

	osg::Camera* _camera;
};

struct ModelMatrixCallback : public osg::Uniform::Callback {
	explicit ModelMatrixCallback(osg::Camera *camera) : _camera(camera) {}

	virtual void operator()(osg::Uniform *uniform, osg::NodeVisitor *nv) {
		osg::Matrixd modelMatrix = osg::computeLocalToWorld(nv->getNodePath());
		uniform->set(modelMatrix);
	}

	osg::Camera *_camera;
};

struct ViewMatrixCallback : public osg::Uniform::Callback {
	explicit ViewMatrixCallback(osg::Camera *camera) : _camera(camera) {}

	virtual void operator()(osg::Uniform *uniform, osg::NodeVisitor *nv) {
		osg::Matrixd viewMatrix = _camera->getViewMatrix();
		uniform->set(viewMatrix);
	}

	osg::Camera *_camera;
};

struct ProjectionMatrixCallback : public osg::Uniform::Callback {
	explicit ProjectionMatrixCallback(osg::Camera *camera) : _camera(camera) {}

	virtual void operator()(osg::Uniform *uniform, osg::NodeVisitor *nv) {
		osg::Matrixd projectionMatrix = _camera->getProjectionMatrix();
		uniform->set(projectionMatrix);
	}

	osg::Camera *_camera;
};
struct CameraEyeCallback : public osg::Uniform::Callback
{
	CameraEyeCallback(osg::Camera* camera) :
		_camera(camera) {
	}

	virtual void operator()(osg::Uniform* uniform, osg::NodeVisitor* /*nv*/) {
		osg::Vec3f eye, center, up;
		_camera->getViewMatrixAsLookAt(eye, center, up);
		osg::Vec4f eye_vec = osg::Vec4f(eye.x(), eye.y(), eye.z(), 1);
		uniform->set(eye_vec);
	}
	osg::Camera* _camera;
};

struct TimerCallback : public osg::Uniform::Callback {
	virtual void operator()(osg::Uniform *uniform, osg::NodeVisitor * /*nv*/) {
		// 获取当前时间
		double currentTime = osg::Timer::instance()->time_s();
		// 更新着色器中的 time uniform
		uniform->set(static_cast<float>(currentTime));
	}
};

}
