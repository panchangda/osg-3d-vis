#include <osg/Texture3D>
#include <osg/Texture2D>
#include <osg/Point>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/TexGen>
#include <osg/StateSet>
#include <osg/BlendFunc>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <osgUtil/Tessellator>
#include <osgText/Font>
#include <osgText/Text>
#include <osg/LineWidth>
#include <string>

class RadarChart {
public:
	RadarChart() {

	}
public:
	osg::ref_ptr<osg::Geode> generateRadar();

};