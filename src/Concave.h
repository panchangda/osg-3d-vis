#pragma once
#include "ConcaveBase.h"
#include <vector>
#include <list>
#include <algorithm>

class concave {
public:
	int MIN_SEARCH_BBOX_SIZE, MAX_SEARCH_BBOX_SIZE;
	float MAX_CONCAVE_ANGLE_COS;

	concave() {
		MIN_SEARCH_BBOX_SIZE = MAX_SEARCH_BBOX_SIZE = 1;
		MAX_CONCAVE_ANGLE_COS = -0.001;
	}

	void getIsoLineConcave(std::vector<Vec2_X>& pointsset, const osg::Vec4f& points_range, std::list<osg::Vec3i>& concave, float maxEdgeLen);

private:
	void _upperTangent(std::vector<Vec2_X>& pointset, std::vector<int>& upper);
	void _lowerTangent(std::vector<Vec2_X>& pointset, std::vector<int>& lower);
	float _cross(Vec2_X& v0, Vec2_X& v1, Vec2_X& v2);
	void _concave(std::vector<Vec2_X>& pointsset, std::list<osg::Vec3i>& convex, float maxEdgeLen, Qtree_E*& edgetree, Qtree_P*& pointtree);
	int _midPoint(Qtree_E*& edgetree, std::list<Vec2_X>& innerpoints, Edge& e0);
};