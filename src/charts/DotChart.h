#pragma once


#include <osg/Geode>

#include<osgDB/WriteFile>
#include <osg/Point>
#include <osg/ShapeDrawable>




class DotChart {
public:
	DotChart(){

	}
public:
	float generateRandom();
	osg::ShapeDrawable* createShpe(osg::Vec3 location, float radius, osg::Vec4 color);
	osg::ref_ptr<osg::Geode> generateDot();
};

