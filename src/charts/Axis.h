#pragma once

#include<osgViewer/Viewer>
#include <osg/Geode>



class Axis {
public:

	Axis() {
	
	}

	osg::ref_ptr<osg::Geode> DrawAxis(float ori_x, float ori_y, float ori_z, float a_x, float a_y, float a_z, float font_size);


};