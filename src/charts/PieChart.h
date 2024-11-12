#pragma once

#include<osgViewer/Viewer>
#include <osg/Geode>
#include<osg/Group>


class PieChart {
public:

	PieChart() {

	}

	osg::ref_ptr<osg::Geode> generatePieChart();

private:
	osg::ref_ptr<osg::Group> root;
};