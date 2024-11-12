#pragma once

#include <osg/Geode>
#include <osg/Geometry>
#include<osg/Group>
#include<osgDB/WriteFile>

#include <osg/Point>

#include <osgText/Font>


class ColumnChart {
public:
	ColumnChart() {

	}
	osg::ref_ptr<osg::Geode> drawQuads(osg::Vec3 location, osg::Vec3 color, float height);
	osg::ref_ptr<osg::Geometry> drawFace(osg::ref_ptr<osg::Vec3Array> v, osg::Vec4 color);
	osg::ref_ptr<osg::Geode> generateColumn();
private:
	osg::ref_ptr<osg::Group> root;
};