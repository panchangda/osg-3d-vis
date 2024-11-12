#pragma once


#include <osg/Geode>
#include <osgGA/TrackballManipulator>


class RadarChart {
public:
	RadarChart() {

	}
public:
	osg::ref_ptr<osg::Geode> generateRadar();

};