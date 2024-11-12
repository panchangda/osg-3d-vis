#pragma once


#include<osgViewer/Viewer>
#include <osg/Geode>
#include<osg/Group>


class PolylineChart {
public:

	PolylineChart() {

	}

	osg::ref_ptr<osg::Geode> generateLineChart();

private:
	osg::ref_ptr<osg::Group> root;
};