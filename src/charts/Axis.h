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


class Axis {
public:

	Axis() {
	
	}

	osg::ref_ptr<osg::Geode> DrawAxis(float ori_x, float ori_y, float ori_z, float a_x, float a_y, float a_z, float font_size);


};