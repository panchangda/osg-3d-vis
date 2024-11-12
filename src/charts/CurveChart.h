#pragma once

#include <osg/Geode>
#include<osg/Group>
#include<osgDB/WriteFile>

class CurveChart {
public:

	CurveChart() {

	}

	osg::ref_ptr<osg::Geode> generateCurveChart();

private:
	osg::ref_ptr<osg::Group> root;
};