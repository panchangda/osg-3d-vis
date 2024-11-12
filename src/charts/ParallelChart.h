#pragma once

#include<osgViewer/Viewer>
#include <osg/Geode>


class ParallelChart {
public:
	ParallelChart() {

	}
public:
	osg::ref_ptr<osg::Geode> generateParallel();

};