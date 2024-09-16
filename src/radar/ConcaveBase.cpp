#include "ConcaveBase.h"
#include <osg/Math>

bool Vec2_X::operator <(const Vec2_X& v1) const
{
	if (v1.x == x)
	{
		return (y < v1.y);
	}
	else {
		return (x < v1.x);
	}
}
bool Vec2_X::operator ==(const Vec2_X& v0) const
{
	return (x == v0.x && y == v0.y);
}
Vec2_X::Vec2_X(float xx, float yy) :x(xx), y(yy)
{

}

Vec2_X::Vec2_X()
{
	x = 0; y = 0;
}

float  Vec2_X::_sqLength(const Vec2_X& v0) const
{
	float dx = (x - v0.x);
	float dy = (y - v0.y);
	return (dx * dx + dy * dy);
}

float  Vec2_X::_cos(const Vec2_X& p0, const Vec2_X& p1)
{
	float dotvalue = (p0.x - x) * (p1.x - x) + (p0.y - y) * (p1.y - y);
	float sqAlen = _sqLength(p0);
	float sqBlen = _sqLength(p1);
	return dotvalue / sqrtf(sqAlen * sqBlen);
}

bool Vec2_X::_bInRange(const osg::Vec4f& range) const
{
	return (x >= range.x() && x <= (range.x() + range.y()) && y >= range.z() && y <= (range.z() + range.w()));
}

int Range::_OverLap(osg::Vec4f& r0, osg::Vec4f& r1)
{
	osg::Vec2f r0max(r0.x() + r0.y(), r0.z() + r0.w());
	osg::Vec2f r1max(r1.x() + r1.y(), r1.z() + r1.w());
	if (r0.x() > r1max.x() || r0max.x() < r1.x() || r0.z() > r1max.y() || r0max.y() < r1.z())
	{
		return -1;
	}
	else if (r0.x() >= r1.x() && r0max.x() <= r1max.x() && r0.z() >= r1.z() && r0max.y() <= r1max.y())
	{
		return 0;
	}
	else {
		return 1;
	}
}

void Range::_Enlarge(osg::Vec4f& r0, Vec2_X& p0)
{
	if (p0.x < r0.x())
	{
		r0.y() += (r0.x() - p0.x);
		r0.x() = p0.x;
	}
	else
	{
		r0.y() = p0.x - r0.x();
	}
	if (p0.y < r0.z())
	{
		r0.w() += (r0.z() - p0.y);
		r0.z() = p0.y;
	}
	else if (p0.y > r0.z() + r0.w()) {
		r0.w() = p0.y - r0.z();
	}
}

void Range::_Enlarge(osg::Vec4f& r0, Edge& e0)
{
	_Enlarge(r0, e0.points[0]);
	_Enlarge(r0, e0.points[1]);
}

Qtree_Node_P::Qtree_Node_P(osg::Vec4f r) :_range(r)
{
	children[0] = NULL; children[1] = NULL; children[2] = NULL; children[3] = NULL;
	points.clear();
}

Qtree_Node_P::~Qtree_Node_P()
{
	_releaseNode(this);
}

void Qtree_Node_P::_releaseNode(Qtree_Node_P* r)
{
	for (int i = 0; i < 4; i++)
	{
		if (r->children[i] != NULL)
		{
			delete r->children[i];
			r->children[i] = NULL;
		}
	}
}

void Qtree_Node_P::AddPoint(Vec2_X& p)
{
	if (children[0] == NULL)
	{
		if (p._bInRange(_range))
		{
			points.push_back(p);
			_divideNode();
		}

	}
	else {
		for (int i = 0; i < 4; i++)
		{
			if (p._bInRange(children[i]->_range))
			{
				children[i]->AddPoint(p);
				return;
			}
		}
	}
}

void Qtree_Node_P::_divideNode()
{
	if (points.size() > MAX_POINTS_IN_ONE_NODE)
	{
		osg::Vec2f childsize(_range.y() * 0.5, _range.w() * 0.5);
		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				osg::Vec4f currange(_range.x() + childsize.x() * x, childsize.x(), _range.z() + childsize.y() * y, childsize.y());
				children[y * 2 + x] = new Qtree_Node_P(currange);
			}
		}
		for (std::list<Vec2_X>::iterator it = points.begin(); it != points.end(); ++it)
		{
			int index = _getIndex(*it);
			children[index]->points.push_back(*it);
		}
		points.clear();

		for (int i = 0; i < 4; i++)
		{
			children[i]->_divideNode();
		}
	}
}

void Qtree_Node_P::RemovePoint(Vec2_X& p)
{
	if (children[0] == NULL)
	{
		for (std::list<Vec2_X>::iterator it = points.begin(); it != points.end();)
		{
			if (*it == p)
			{
				it = points.erase(it);
			}
			else {
				++it;
			}
		}
	}
	else {
		for (int i = 0; i < 4; i++)
		{
			if (p._bInRange(children[i]->_range))
			{
				children[i]->RemovePoint(p);
				return;
			}
		}
	}
}

void Qtree_Node_P::RangePoints(osg::Vec4f& range, std::list<Vec2_X>& ps)
{
	if (children[0] == NULL)
	{
		for (std::list<Vec2_X>::iterator it = points.begin(); it != points.end(); ++it)
		{
			if (it->_bInRange(range))
			{
				ps.push_back(*it);
			}
		}
	}
	else {
		int overlapflag = Range::_OverLap(_range, range);
		if (overlapflag == 0)
		{
			AddAllToList(ps);
		}
		else if (overlapflag == 1)
		{
			for (int i = 0; i < 4; i++)
			{
				children[i]->RangePoints(range, ps);
			}
		}
	}
}

void Qtree_Node_P::AddAllToList(std::list<Vec2_X>& ps)
{
	if (children[0] == NULL)
	{
		for (std::list<Vec2_X>::iterator it = points.begin(); it != points.end(); ++it)
		{
			ps.push_back(*it);
		}
	}
	else {
		for (int i = 0; i < 4; i++)
		{
			children[i]->AddAllToList(ps);
		}
	}
}

Qtree_P::Qtree_P(osg::Vec4f r)
{
	root = new Qtree_Node_P(r);
}

Qtree_P::~Qtree_P()
{
	delete root;
	root = NULL;
}

void Qtree_P::AddPoint(Vec2_X& p)
{
	root->AddPoint(p);
}

void Qtree_P::RemovePoint(Vec2_X& p)
{
	root->RemovePoint(p);
}

void Qtree_P::RangePoints(osg::Vec4f& range, std::list<Vec2_X>& ps)
{
	root->RangePoints(range, ps);
}

Qtree_Node_E::Qtree_Node_E(osg::Vec4f r) :_range(r)
{
	children[0] = NULL; children[1] = NULL; children[2] = NULL; children[3] = NULL;
	edges.clear();
}

Qtree_Node_E::~Qtree_Node_E()
{
	_releaseNode(this);
}

Edge::Edge(Vec2_X& p0, Vec2_X& p1)
{
	points[0] = p0;
	points[1] = p1;
}

Edge::Edge(float x0, float y0, float x1, float y1)
{
	points[0] = Vec2_X(x0, y0);
	points[1] = Vec2_X(x1, y1);
}

bool Edge::operator ==(const Edge& e0) const
{
	if (points[0] == e0.points[0] && points[1] == e0.points[1])
	{
		return true;
	}
	else if (points[0] == e0.points[1] && points[1] == e0.points[0])
	{
		return true;
	}
	return false;
}

bool Edge::operator <(const Edge& e0) const
{
	if (std::min(points[0].x, points[1].x) < std::min(e0.points[0].x, e0.points[1].x))
	{
		return true;
	}
	else {
		return false;
	}
}

bool Edge::operator >(const Edge& e0) const
{
	if (*this < e0 || *this == e0)
	{
		return false;
	}
	else {
		return true;
	}
}

bool Edge::_bShareEnd(const Edge& e0) const
{
	if (points[0] == e0.points[0] || points[1] == e0.points[0] || points[0] == e0.points[1] || points[1] == e0.points[1])
	{
		return true;
	}
	return false;
}

bool Edge::_bInRange(const osg::Vec4f& range) const
{
	bool p0in = points[0]._bInRange(range);
	bool p1in = points[1]._bInRange(range);
	if (p0in && p1in)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Edge::_bOverLap(const osg::Vec4f& range) const
{
	/*if (_bInRange(range))
	{
		return true;
	}*/
	bool p0in = points[0]._bInRange(range);
	bool p1in = points[1]._bInRange(range);
	if (p0in || p1in)
	{
		return true;
	}
	if (_bIntersect(Edge(range.x(), range.z(), range.x() + range.y(), range.z())) ||
		_bIntersect(Edge(range.x(), range.z(), range.x(), range.z() + range.w())) ||
		_bIntersect(Edge(range.x() + range.y(), range.z(), range.x() + range.y(), range.z() + range.w())) ||
		_bIntersect(Edge(range.x(), range.z() + range.w(), range.x() + range.y(), range.z() + range.w())))
	{
		return true;
	}
	return false;
}

osg::Vec4f Edge::_getRect(float len) const
{
	static osg::Vec2f ps[4];
	ps[0] = osg::Vec2f(points[0].x, points[0].y);
	ps[1] = osg::Vec2f(points[1].x, points[1].y);

	osg::Vec2f dir = ps[0] - ps[1];
	dir = osg::Vec2f(-dir.y(), dir.x());
	ps[2] = ps[0] + dir * len;
	ps[3] = ps[1] + dir * len;
	float xmin = std::min(std::min(ps[0].x(), ps[1].x()), std::min(ps[2].x(), ps[3].x()));
	float xmax = std::max(std::max(ps[0].x(), ps[1].x()), std::max(ps[2].x(), ps[3].x()));
	float ymin = std::min(std::min(ps[0].y(), ps[1].y()), std::min(ps[2].y(), ps[3].y()));
	float ymax = std::max(std::max(ps[0].y(), ps[1].y()), std::max(ps[2].y(), ps[3].y()));

	return osg::Vec4f(xmin, xmax - xmin, ymin, ymax - ymin);
}

bool Edge::_bIntersect(const Edge& e0) const
{
	osg::Vec2f s0p0(points[0].x, points[0].y);
	osg::Vec2f s0p1(points[1].x, points[1].y);
	osg::Vec2f s1p0(e0.points[0].x, e0.points[0].y);
	osg::Vec2f s1p1(e0.points[1].x, e0.points[1].y);
	osg::Vec2f d1 = s0p1 - s0p0;
	osg::Vec2f d2 = s1p1 - s1p0;
	osg::Vec2f u = s0p0 - s1p0;

	float corssvalue = 1.0f / (-d2.x() * d1.y() + d1.x() * d2.y());
	float s = (-d1.y() * u.x() + d1.x() * u.y()) * corssvalue;
	float t = (-d2.y() * u.x() + d2.x() * u.y()) * corssvalue;
	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		return true;
	}
	return false;/*
	float x1 = points[0].x, y1 = points[0].y,
		x2 = points[1].x, y2 = points[1].y,
		x3 = e0.points[0].x, y3 = e0.points[0].y,
		x4 = e0.points[1].x, y4 = e0.points[1].y;
	return (_ccw(x1, y1, x3, y3, x4, y4) != _ccw(x2, y2, x3, y3, x4, y4)) &&
		(_ccw(x1, y1, x2, y2, x3, y3) != _ccw(x1, y1, x2, y2, x4, y4));*/
}

bool Edge::_ccw(float x1, float y1, float x2, float y2, float x3, float y3) const
{
	float cw = ((y3 - y1) * (x2 - x1)) - ((y2 - y1) * (x3 - x1));
	return cw > 0 ? true : cw < 0 ? false : true;
}

void Qtree_Node_E::AddEdge(Edge& e)
{
	if (children[0] == NULL)
	{
		osg::Vec2f childsize(_range.y() * 0.5, _range.w() * 0.5);
		for (int x = 0; x < 2; x++)
		{
			for (int y = 0; y < 2; y++)
			{
				osg::Vec4f currange(_range.x() + childsize.x() * x, childsize.x(), _range.z() + childsize.y() * y, childsize.y());
				children[y * 2 + x] = new Qtree_Node_E(currange);
			}
		}
	}
	int i = 0;
	for (; i < 4; i++)
	{
		if (e._bInRange(children[i]->_range))
		{
			children[i]->AddEdge(e);
			break;
		}
	}
	if (i == 4)
	{
		edges.push_back(e);
	}
}

//void Qtree_Node_E::_divideNode()
//{
//	if (children[0] == NULL && edges.size() > MAX_POINTS_IN_ONE_NODE)
//	{
//		osg::Vec2f childsize(_range.y * 0.5, _range.w * 0.5);
//		for (int x = 0; x < 2; x++)
//		{
//			for(int y = 0; y < 2; y++)
//			{
//				osg::Vec4f currange(_range.x + childsize.x * x, childsize.x, _range.z + childsize.y * y,  childsize.y);
//				children[y * 2 + x] = new Qtree_Node_E(currange);
//			}
//		}
//		for (std::list<Edge>::iterator it = edges.begin(); it != edges.end(); )
//		{
//			static bool Range_index[4];
//			int tempcount = 0;
//			for (int i = 0; i < 4; i++)
//			{
//				if (it->_bInRange(children[i]->_range))
//				{
//					Range_index[i] = true;
//					tempcount++;
//				}
//			}
//			if (tempcount >= 2)
//			{
//				++it;
//			}else{
//				for (int i = 0; i < 4; i++)
//				{
//					if (Range_index[i])
//					{
//						children[i]->edges.push_back(*it);
//					}
//				}
//				it = edges.erase(it);
//			}
//		}
//
//		for (int i = 0; i < 4; i++)
//		{
//			children[i]->_divideNode();
//		}
//	}
//}

void Qtree_Node_E::RemoveEdge(const Edge& e)
{
	if (children[0] != NULL)
	{
		int i = 0;
		for (; i < 4; i++)
		{
			if (e._bInRange(children[i]->_range))
			{
				children[i]->RemoveEdge(e);
				break;
			}
		}
		if (i == 4)
		{
			for (std::list<Edge>::iterator it = edges.begin(); it != edges.end();)
			{
				if (*it == e)
				{
					it = edges.erase(it);
				}
				else {
					++it;
				}
			}
		}
	}
}

void Qtree_Node_E::RangeEdges(osg::Vec4f& range, std::set<Edge>& es)
{
	for (std::list<Edge>::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		if (it->_bOverLap(range))
		{
			es.insert(*it);
		}
	}
	if (children[0] != NULL)
	{
		for (int i = 0; i < 4; i++)
		{
			int overlapflag = Range::_OverLap(children[i]->_range, range);
			if (overlapflag == 0)
			{
				AddAllToList(es);
			}
			else if (overlapflag == 1)
			{
				children[i]->RangeEdges(range, es);
			}
		}
	}
}

void Qtree_Node_E::AddAllToList(std::set<Edge>& es)
{
	for (std::list<Edge>::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		es.insert(*it);
	}
	if (children[0] != NULL)
	{
		for (int i = 0; i < 4; i++)
		{
			children[i]->AddAllToList(es);
		}
	}
}

void Qtree_Node_E::_releaseNode(Qtree_Node_E* r)
{
	for (int i = 0; i < 4; i++)
	{
		if (r->children[i] != NULL)
		{
			delete r->children[i];
			r->children[i] = NULL;
		}
	}
}

bool Qtree_Node_E::Intersect(const Edge& e)
{
	for (std::list<Edge>::iterator it = edges.begin(); it != edges.end(); ++it)
	{
		if (it->_bShareEnd(e))
		{
			continue;
		}
		if (it->_bIntersect(e))
		{
			return true;
		}
	}
	if (children[0] != NULL)
	{
		for (int i = 0; i < 4; i++)
		{
			if (e._bOverLap(children[i]->_range))
			{
				if (children[i]->Intersect(e))
				{
					return true;
				}
			}
		}
	}
	return false;/*
				 osg::Vec4f range = e._getRect(0.1f);
				 std::set<Edge> es; es.clear();
				 RangeEdges(range, es);
				 for (std::set<Edge>::iterator it = es.begin(); it != es.end(); ++it)
				 {
				 if (it->_bShareEnd(e))
				 {
				 continue;
				 }
				 if (it->_bIntersect(e))
				 {
				 return true;
				 }
				 }
				 return false;*/
}

Qtree_E::Qtree_E(osg::Vec4f r)
{
	root = new Qtree_Node_E(r);
}

Qtree_E::~Qtree_E()
{
	delete root;
	root = NULL;
}

void Qtree_E::AddEdge(Edge& e)
{
	root->AddEdge(e);
}

void Qtree_E::RangeEdges(osg::Vec4f& range, std::set<Edge>& es)
{
	root->RangeEdges(range, es);
}

void Qtree_E::RemoveEdge(const Edge& e)
{
	root->RemoveEdge(e);
}

bool Qtree_E::Intersect(const Edge& e)
{
	return root->Intersect(e);
}