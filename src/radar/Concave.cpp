#include "Concave.h"


void concave::_concave(std::vector<Vec2_X> &pointsset, std::list<osg::Vec3i> &convex, float maxEdgeLen, Qtree_E* &edgetree, Qtree_P* &pointtree)
{
	bool midPointInserted = false;
	std::list<Vec2_X> innerpoints;
	while (true)
	{
		midPointInserted = false;
		for (std::list<osg::Vec3i>::iterator it = convex.begin(); it != convex.end(); )
		{
			//�����ʶΪ-2 ��ʾ�����߲����ٳ������ˣ����������˻����Ѿ��ҹ�û�к��ʵĵ��ˣ�
			if (it->z() == -2) //final edge
			{
				++it;
				continue;
			}

			Edge cur_edge(pointsset[it->x()], pointsset[it->y()]);
			float edge_length = cur_edge._Length();

			//�ߵĳ���С����󳤶ȣ��������ҵ��ˣ� maxEdgeLen ���Կ�������
			if (edge_length < maxEdgeLen) //final edge
			{
				it->z() = -2;
				++it;
				continue;
			}

			int midpoint_index = -1;
			//try find midpoint
			//���Դӱߵ�һ��һ����Χ�ڵĵ㼯���ҵ�����ʵ��е㣬 MIN_SEARCH_BBOX_SIZE MAX_SEARCH_BBOX_SIZE���Կ��Ÿ��û�����
			for (int i = MIN_SEARCH_BBOX_SIZE; i <= MAX_SEARCH_BBOX_SIZE && midpoint_index == -1; i++)
			{
				//midpoint
				innerpoints.clear();
				osg::Vec4f range = cur_edge._getRect(-i);
				pointtree->RangePoints(range, innerpoints);

				midpoint_index = _midPoint(edgetree, innerpoints, cur_edge);
			}

			//insert two segments, delete one segment, delete midpoint midPointInserted
			//����Ҳ�����ѡ�㣬���ñ�ʶλ �Ժ��ٳ���
			if (midpoint_index == -1)
			{
				it->z() = -2;
				++it;
			}
			else {  //�ҵ��Ļ�������������γ� �µ������� ��ԭ��Ϊ(p0, p1)�� ������(p0, midpoint) �� (midpoint, p1))����ؼ��ٽṹҲ������
				//etree
				edgetree->RemoveEdge(cur_edge);
				Edge* edge1 = new Edge(cur_edge.points[0], pointsset[midpoint_index]);
				edgetree->AddEdge(*edge1);
				Edge* edge2 = new Edge(pointsset[midpoint_index], cur_edge.points[1]);
				edgetree->AddEdge(*edge2);
				//pointtree
				pointtree->RemovePoint(pointsset[midpoint_index]);
				//convex
				convex.insert(it, osg::Vec3i(it->x(), midpoint_index, -1));
				convex.insert(it, osg::Vec3i(midpoint_index, it->y(), -1));
				it = convex.erase(it);
				//midPointInserted
				midPointInserted = true;
			}
		}
		//������еı߶��Թ��� û�п��Լ���ľͽ���
		if (!midPointInserted)
		{
			break;
		}
	}

}

int concave::_midPoint(Qtree_E* &edgetree, std::list<Vec2_X> &innerpoints, Edge &e0)
{
	int ret = -1;
	float angle1cos = MAX_CONCAVE_ANGLE_COS;
	float angle2cos = MAX_CONCAVE_ANGLE_COS;
	float a1cos, a2cos;
	for (std::list<Vec2_X>::iterator it = innerpoints.begin(); it != innerpoints.end(); ++it)
	{
		Vec2_X& curpoint = *it;
		a1cos = e0.points[0]._cos(e0.points[1], curpoint);
		a2cos = e0.points[1]._cos(e0.points[0], curpoint);

		//�ڵ㼯���ҵ�һ�����ŵ㣨�õ����֮�������ɵ�������ԭ�߼н���С�ģ����������ɵ����߲��������е����б��н��棩
		if (a1cos > angle1cos &&
			a2cos > angle2cos &&
			!edgetree->Intersect(Edge(e0.points[0], curpoint)) &&
			!edgetree->Intersect(Edge(curpoint, e0.points[1])))
		{
			angle1cos = a1cos;
			angle2cos = a2cos;
			ret = curpoint.points_index;
		}
	}
	return ret;
}

void concave::getIsoLineConcave(std::vector<Vec2_X> &pointsset, const osg::Vec4f &points_range, std::list<osg::Vec3i> &concave, float maxEdgeLen)
{
	sort(pointsset.begin(), pointsset.end()); //��X��������
	std::set<int> innerPoints;
	for (int i = 0; i < pointsset.size(); i++)
	{
		pointsset[i].points_index = i;
		innerPoints.insert(i);
	}
	std::vector<int> upper;
	std::vector<int> lower;

	//����㼯��͹��
	_upperTangent(pointsset, upper);
	_lowerTangent(pointsset, lower);
	lower.insert(lower.begin(), upper.begin(), upper.end());

	//����͹���㼯�ĵ㲻��δ�������ĺ�ѡ�㣬�Ӽ�����ɾ��
	for (int i = 0; i < lower.size(); i++)
	{
		innerPoints.erase(lower[i]);
	}

	//�����ѡ����Ĳ�������������
	Qtree_P* tree = new Qtree_P(points_range);
	for (std::set<int>::iterator it = innerPoints.begin(); it != innerPoints.end(); ++it)
	{
		tree->AddPoint(pointsset[*it]);
	}

	//�������б߼����Ĳ������������ٶ�
	Qtree_E* etree = new Qtree_E(points_range);
	concave.clear();
	for (int i = 0; i < lower.size() - 1; i++)
	{
		Edge* edge = new Edge(pointsset[lower[i]], pointsset[lower[i + 1]]);
		etree->AddEdge(*edge);
		concave.push_back(osg::Vec3i(lower[i], lower[i + 1], -1));
	}
	Edge* edge = new Edge(pointsset[lower.back()], pointsset[lower.front()]);
	etree->AddEdge(*edge);
	concave.push_back(osg::Vec3i(lower.back(), lower.front(), -1));

	//�����������������concave�У� ��ʼconcave��͹���ı߼���ѭ������concave�мӺ�ѡ��
	_concave(pointsset, concave, maxEdgeLen, etree, tree);

	delete etree;
	delete tree;
}

void concave::_lowerTangent(std::vector<Vec2_X> &pointset, std::vector<int> &lower)
{
	lower.clear();
	int i = pointset.size() - 1;
	for (; i >= 0; i--)
	{
		while (lower.size() >= 2 && (_cross(pointset[lower[lower.size() - 2]], pointset[lower[lower.size() - 1]], pointset[i]) <= 0))
		{

			lower.pop_back();
		}
		lower.push_back(i);
	}
	lower.pop_back();
}

void concave::_upperTangent(std::vector<Vec2_X> &pointset, std::vector<int> &upper)
{
	upper.clear();
	for (int i = 0; i < pointset.size(); i++)
	{
		while (upper.size() >= 2 && (_cross(pointset[upper[upper.size() - 2]], pointset[upper[upper.size() - 1]], pointset[i]) <= 0))
		{
			upper.pop_back();
		}
		upper.push_back(i);
	}
	upper.pop_back();
}

float concave::_cross(Vec2_X &v0, Vec2_X &v1, Vec2_X &v2)
{
	return (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);
}
