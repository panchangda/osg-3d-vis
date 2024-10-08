#pragma once

#include<osgViewer/Viewer>
#include <osg/Geode>
#include <osg/Geometry>
#include<osg/Group>
#include<osgDB/ReadFile>
#include<osgDB/WriteFile>
#include<osgUtil/Optimizer>
#include <osgGA/TrackballManipulator>
#include <cstdio>
#include <numbers>
#include <iostream>
#include <cmath>
#include <osgViewer/Viewer>
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