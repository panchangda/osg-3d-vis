#pragma once

#include <list>
#include <set>
#include <osg/Drawable>

struct Vec2_X {
	float x, y;
	int points_index;
public:
	bool operator <(const Vec2_X& v1) const;
	bool operator ==(const Vec2_X& v0) const;
	Vec2_X(float xx, float yy);
	Vec2_X();
	float _sqLength(const Vec2_X& v0) const;
	float _cos(const Vec2_X& p0, const Vec2_X& p1);
	bool _bInRange(const osg::Vec4f& range) const;
};

struct Edge {
	Vec2_X points[2];
	Edge(Vec2_X& p0, Vec2_X& p1);
	Edge(float x0, float y0, float x1, float y1);
	Edge() {}
	bool operator ==(const Edge& e0) const;
	bool operator <(const Edge& e0) const;
	bool operator >(const Edge& e0) const;
	bool _bInRange(const osg::Vec4f& range) const;
	bool _bOverLap(const osg::Vec4f& range) const;
	bool _bIntersect(const Edge& e0) const;
	bool _bShareEnd(const Edge& e0) const;
	float _sqLength() const
	{
		return points[0]._sqLength(points[1]);
	}
	float _Length() const
	{
		return sqrtf(_sqLength());
	}
	/*vec4f _getRect()
	{
		return vec4f(std::min(points[0].x, points[1].x), abs(points[0].x - points[1].x),std::min(points[0].y, points[1].y), abs(points[0].y - points[1].y));
	}*/
	osg::Vec4f _getRect(float len) const;
private:
	bool _ccw(float x1, float y1, float x2, float y2, float x3, float y3) const;
};

struct Range {
	inline static int _OverLap(osg::Vec4f& r0, osg::Vec4f& r1);
	static void _Enlarge(osg::Vec4f& r0, Vec2_X& p0);
	static void _Enlarge(osg::Vec4f& r0, Edge& e0);
};


#define MAX_POINTS_IN_ONE_NODE 8
#define MAX_EDGE_TREE_DEPTH 13

struct Qtree_Node_P {
	Qtree_Node_P* children[4];
	osg::Vec4f _range;   //x xmin, y xsize, z ymin, w ysize
	std::list<Vec2_X> points;
public:
	Qtree_Node_P(osg::Vec4f r);
	~Qtree_Node_P();
	void AddPoint(Vec2_X& p);
	void RemovePoint(Vec2_X& p);
	void RangePoints(osg::Vec4f& range, std::list<Vec2_X>& ps);
	void _divideNode();
private:
	int _getIndex(Vec2_X& p)
	{
		int ret = 0;
		ret += (p.x - _range.x() > (_range.y() * 0.5));
		ret += ((int)(p.y - _range.z() > (_range.w() * 0.5))) << 1;
		return ret;
	}
	void AddAllToList(std::list<Vec2_X>& ps);
	void _releaseNode(Qtree_Node_P* r);

};

struct Qtree_P {
private:
	Qtree_Node_P* root;
public:
	Qtree_P(osg::Vec4f r);
	~Qtree_P();
	void AddPoint(Vec2_X& p);
	void RemovePoint(Vec2_X& p);
	void RangePoints(osg::Vec4f& range, std::list<Vec2_X>& ps);

};

struct Qtree_Node_E {
private:
	Qtree_Node_E* children[4];
	osg::Vec4f _range;
	std::list<Edge> edges;

public:
	Qtree_Node_E(osg::Vec4f r);
	~Qtree_Node_E();
	void AddEdge(Edge& e);
	void RemoveEdge(const Edge& e);
	void RangeEdges(osg::Vec4f& range, std::set<Edge>& es);
	void _divideNode();
	bool Intersect(const Edge& e);
private:
	void AddAllToList(std::set<Edge>& es);
	void _releaseNode(Qtree_Node_E* r);
};

struct Qtree_E {
private:
	Qtree_Node_E* root;
public:
	Qtree_E(osg::Vec4f r);
	~Qtree_E();
	void AddEdge(Edge& e);
	void RemoveEdge(const Edge& e);
	void RangeEdges(osg::Vec4f& range, std::set<Edge>& es);
	bool Intersect(const Edge& e);
};