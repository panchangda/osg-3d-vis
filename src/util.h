#pragma once
#include <osgViewer/Viewer>

namespace osg_3d_vis {


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
		double _minHeight, double _maxHeight)
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

inline void llh2xyz_Ellipsoid(double _lat, double _lon, double _h, double& x, double& y, double& z) {

	double lat = _lat;
	double lon = _lon;
	double h = _h;
	auto pEllModel = new osg::EllipsoidModel();
	pEllModel->convertLatLongHeightToXYZ(lat, lon, h, x, y, z);
}

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

}