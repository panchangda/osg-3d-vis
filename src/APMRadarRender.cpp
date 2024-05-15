#include "APMRadarRender.h"
#include <algorithm>
#include <set>
#include <list>
#include <string>
#include <iostream>

using namespace std;


ApmRadarPara::ApmRadarPara()
{
	Pt = 360000;
	G = 28.5;
	randa = 0.33;
	area = 20;
	K = 1.38e-23;
	Ts = 290;
	Bn = 8e3;
	F0 = 12.59;
	Pfa = 1e-6;
	N = 93;

	D_At_Points = NULL;
	Loss_AtPoints = NULL;

	UpdatePara();

	_concave = new concave();
	_concave->MAX_CONCAVE_ANGLE_COS = -0.001;
	_concave->MAX_SEARCH_BBOX_SIZE = 1;
	_concave->MIN_SEARCH_BBOX_SIZE = 1;
}

ApmRadarPara::~ApmRadarPara()
{
	if (Loss_AtPoints != NULL)
	{
		delete[] Loss_AtPoints;
		Loss_AtPoints = NULL;
	}
	if (D_At_Points != NULL)
	{
		delete[] D_At_Points;
		D_At_Points = NULL;
	}
	if (_concave != NULL)
	{
		delete _concave;
		_concave = NULL;
	}
}

void ApmRadarPara::SetMaxAngle_length_box(float a, float l, float maxb, float minb)
{
	_concave->MAX_CONCAVE_ANGLE_COS = cosf(a / 180 * 3.1415926);
	_concave->MAX_SEARCH_BBOX_SIZE = maxb;
	_concave->MIN_SEARCH_BBOX_SIZE = minb;
	edge_length = l;
}

void ApmRadarPara::UpdatePara()
{
	Aeff = 10 * log(Pt * area * randa * randa / pow_4_pi_3) / log_10 + G * 2;
	Beff = 10 * log(K * Ts * Bn) / log_10 + F0;
}

void ApmRadarPara::UpdateDValue()
{
	int count = hcount * dcount;
	for (int i = 0; i < count; ++i)
	{
		D_At_Points[i] = getRadarDFromLFR(Loss_AtPoints[i].x(), Loss_AtPoints[i].y(), Loss_AtPoints[i].z());
	}
}

void ApmRadarPara::MakeRGridFromFile(const string fn)
{
	string str_maxh("Maximum output height (m) =");
	string str_maxr("Maximum output range (km) =");
	string str_hcount(" Number of output height points =");
	string str_rcount(" Number of output range points =");
	ifstream f_in(fn);
	if (!f_in.is_open())
	{
		return;
	}
	std::string line;
	int headcount = 0;
	while (getline(f_in, line))
	{
		if (line.length() < 33)
		{
			continue;
		}
		if (line.substr(0, str_maxh.length()) == str_maxh)
		{
			sscanf(line.substr(str_maxh.length() + 1).c_str(), "%f", &hmax);
			hmax *= 0.001;
			headcount++;
		}
		else if (line.substr(0, str_maxr.length()) == str_maxr)
		{
			sscanf(line.substr(str_maxr.length() + 1).c_str(), "%f", &dmax);
			headcount++;
		}
		else if (line.substr(0, str_hcount.length()) == str_hcount)
		{
			sscanf(line.substr(str_hcount.length() + 1).c_str(), "%d", &hcount);
			headcount++;
		}
		else if (line.substr(0, str_rcount.length()) == str_rcount)
		{
			sscanf(line.substr(str_rcount.length() + 1).c_str(), "%d", &dcount);
			headcount++;
		}
		if (headcount == 4)
		{
			break;
		}
	}
	if (D_At_Points != NULL)
	{
		delete D_At_Points;
		D_At_Points = NULL;
	}
	D_At_Points = new float[hcount * dcount];
	if (Loss_AtPoints != NULL)
	{
		delete Loss_AtPoints;
		Loss_AtPoints = NULL;
	}
	Loss_AtPoints = new osg::Vec3f[hcount * dcount];
	dstep = dmax / dcount;
	hstep = hmax / hcount;
	int curcount = 0;
	while (getline(f_in, line))
	{
		if (line.length() < 13)
		{
			continue;
		}
		if (line.substr(0, 13) == "range in km =")
		{
			double cur_dist, height, loss, pfac;
			sscanf(line.substr(14).c_str(), "%lf", &cur_dist);

			getline(f_in, line);
			getline(f_in, line);

			while (getline(f_in, line))
			{
				if (line.length() < 13)
				{
					break;
				}
				sscanf(line.c_str(), " %lf %lf %lf", &height, &loss, &pfac);
				height *= 0.001;
				Loss_AtPoints[curcount++] = osg::Vec3f(loss, pfac, sqrt(cur_dist * cur_dist + height * height));
			}
		}
	}
	f_in.close();
}

void ApmRadarPara::getTestPoints(std::vector<osg::Vec3f>& points)
{
	points.clear();
	for (int x = 0; x < dcount; x++)
	{
		for (int y = 0; y < hcount; y++)
		{
			points.push_back(osg::Vec3f(x * dstep, y * hstep * 10, D_At_Points[x * hcount + y]));
		}
	}
}
void ApmRadarPara::getIsoLineConcave(std::vector<osg::Vec2f>& line, double pd, float concavity)
{
	line.clear();
	m_curd = getDFromPd(pd);
	std::vector<Vec2_X> pointsset;
	pointsset.clear();
	_makePointSet(pointsset, m_curd);

	std::list<osg::Vec3i> pointlist_line;
	//获取凹包线，参数分别为：点集合；点集范围（最小x，x方向的size，最小y，y方向的size），凹包结果，最短边aaa   
	_concave->getIsoLineConcave(pointsset, osg::Vec4f(-1, dcount, -1, hcount), pointlist_line, edge_length);


	for (std::list<osg::Vec3i>::iterator it = pointlist_line.begin(); it != pointlist_line.end(); ++it)
	{
		Vec2_X& curpoint = pointsset[it->x()];
		Vec2_X& nexpoint = pointsset[it->y()];
		line.push_back(osg::Vec2f(curpoint.x * dstep, curpoint.y * hstep * 10));
	}

	//// 遍历pointsset
	//for (int i = 0; i < pointsset.size(); i++)
	//{
	//	Vec2_X& curpoint = pointsset[i];
	//	line.push_back(osg::Vec2f(curpoint.x * dstep, curpoint.y * hstep * 10));
	//}

}


float ApmRadarPara::getDFromPd(double pd)
{
	double t = 0.9 * (2 * pd - 1);
	double gd = 1.231 * t / sqrt(1 - t * t);
	double gfa = 2.36 * (sqrt(-log(Pfa) / log_10)) - 1.02;
	double X = (gd + gfa) * (gd + gfa);
	double d0 = X * (1 + sqrt(1 + 16 * N / X)) / (4 * N);

	return (float)d0;
}
float ApmRadarPara::getRadarDFromLFR(double l, double f, double r)
{
	double pr = GetAeff() + f + f - l - 40 * log(r) / log_10;
	double curd = pr - GetBeff();

	return (float)curd;
}

void ApmRadarPara::_makePointSet(std::vector<Vec2_X>& pointsset, float curD)
{
	for (int y = 0; y < hcount; y++)
	{
		if (D_At_Points[y] > curD)
		{
			pointsset.push_back(Vec2_X(0.0001, y));
		}
		for (int x = 1; x < dcount; x++)
		{
			float d0 = D_At_Points[x * hcount - hcount + y];
			float d1 = D_At_Points[x * hcount + y];

			//curindex.push_back((x - 1 + rate));
			if ((d0 > curD && d1 < curD) || (d0 < curD && d1 > curD))
			{
				float rate = (curD - d0) / (d1 - d0);
				rate = (x - 1) + rate;
				pointsset.push_back(Vec2_X(rate, y));
			}
		}
		if (D_At_Points[(dcount - 1) * hcount + y] > curD)
		{
			pointsset.push_back(Vec2_X(dcount - 1, y));
		}
	}
}

RadarRender::RadarRender()
{
	radar = new ApmRadarPara();

	/*m_lines_normals = new Mesh<vec4f, unsigned short>(LINES, GPU_STATIC);
	m_lines_normals->addAttributeType(0, 2, A32F, false);
	m_lines_normals->addAttributeType(1, 2, A32F, true);

	m_grid_points = new Mesh<vec3f, unsigned int>(POINTS, GPU_STATIC);
	m_grid_points->addAttributeType(0, 3, A32F, false);

	m_pRadarGridPointsRender = ProgramManager::StaticProgramManager.getProgram("./media/shaders/RadarGridPoints");
	m_pRadarRoundScanRender = ProgramManager::StaticProgramManager.getProgram("./media/shaders/RadarRoundScan");
	m_pClipPlaneRender = ProgramManager::StaticProgramManager.getProgram("./media/shaders/RadarClipPlane");
	m_pRadarRenderLine = ProgramManager::StaticProgramManager.getProgram("./media/shaders/RadarLineWithNormal");
	m_pRenderWithColor = ProgramManager::StaticProgramManager.getProgram("./media/shaders/RenderWithColor");*/

	m_curpd = 0.5;

	pointFlag = lineFlag = surfaceFlag = true;

}

RadarRender::~RadarRender()
{
	delete radar;
}

//const float startangle = (0.0 / 180.0) * M_PI;
//const float endangle = (360.0 / 180.0) * M_PI;
//const float stepangle = (endangle - startangle) / 20.0;

//void RadarRender::renderRadar3d(ptr<FrameBuffer> fb, const mat4f& mvp)
//{
//	vec4f curcolor = getColorFromPd(m_curpd);
//	m_pRadarRoundScanRender->getUniformMatrix4f("mvp")->setMatrix(mvp);
//	m_pRadarRoundScanRender->getUniform4f("maincolor")->set(curcolor);
//	m_pRadarRoundScanRender->getUniform1f("startangle")->set(startangle);
//	m_pRadarRoundScanRender->getUniform1f("stepangle")->set(stepangle);
//
//	if (surfaceFlag)
//	{
//		fb->setBlend(true, BlendEquation::ADD, BlendArgument::SRC_ALPHA, BlendArgument::ONE_MINUS_SRC_ALPHA);
//		fb->draw(m_pRadarRoundScanRender, *m_lines_normals);
//		fb->setBlend(false);
//	}
//	if (pointFlag)
//	{
//		m_pRadarGridPointsRender->getUniformMatrix4f("mvp")->setMatrix(mvp);
//		m_pRadarGridPointsRender->getUniform1f("curd")->set(radar->m_curd);
//		fb->draw(m_pRadarGridPointsRender, *m_grid_points);
//	}
//	if (lineFlag)
//	{
//		m_pRadarRenderLine->getUniformMatrix4f("mvp")->setMatrix(mvp);
//		m_pRadarRenderLine->getUniform4f("maincolor")->set(curcolor);
//		fb->draw(m_pRadarRenderLine, *m_lines_normals);
//	}
//
//	fb->setDepthTest(false);
//	m_pRenderWithColor->getUniformMatrix4f("ModelViewPerspectiveMatrix")->setMatrix(mvp);
//	m_pRenderWithColor->getUniform4f("color")->set(vec4f(1.0, 0.0, 0, 1.0));
//	fb->draw(m_pRenderWithColor, *m_x_line);
//	fb->setPointSize(5);
//	fb->draw(m_pRenderWithColor, *m_x_point);
//	fb->setPointSize(1);
//	m_pRenderWithColor->getUniform4f("color")->set(vec4f(0.0, 1.0, 0, 1.0));
//	fb->draw(m_pRenderWithColor, *m_y_line);
//	fb->setPointSize(5);
//	fb->draw(m_pRenderWithColor, *m_y_point);
//	fb->setPointSize(1);
//	m_pRenderWithColor->getUniform4f("color")->set(vec4f(0.0, 0.0, 1.0, 1.0));
//	fb->draw(m_pRenderWithColor, *m_z_line);
//	fb->setPointSize(5);
//	fb->draw(m_pRenderWithColor, *m_z_point);
//	fb->setPointSize(1);
//	fb->setDepthTest(true);
//}

void RadarRender::GenerateIsoMesh(double pd, bool isround, bool isSymmetryX)
{
	std::vector<osg::Vec2f> points;
	points.clear();
	radar->getIsoLineConcave(points, pd, 20);

	m_lines_normals->clear();
	m_lines_normals->push_back(osg::Vec4f(points[0].x(), points[0].y(), 1.0, 0));
	for (int i = 1; i < points.size() - 1; i++)
	{
		if (i == 30)
		{
			int xuyibo = 0;
		}
		osg::Vec2f vec0 = (points[i] - points[i - 1]);
		vec0.normalize();
		osg::Vec2f vec1 = (points[i] - points[i + 1]);
		vec1.normalize();
		osg::Vec2f nor = (vec0 + vec1);
		double norlength = nor.length();
		osg::Vec2f line_normal(vec0.y(), -vec0.x());
		if (norlength < 0.0001)
		{
			nor = line_normal;
		}
		else {
			nor /= norlength;
			if (nor * line_normal < 0)
			{
				nor = -nor;
			}
		}
		m_lines_normals->push_back(osg::Vec4f(points[i].x(), points[i].y(), nor.x(), nor.y()));
	}
	//for (int i = 1; i < m_lines_normals->size(); i++)
	//{
	//	m_lines_normals->addIndice(i - 1);
	//	m_lines_normals->addIndice(i);
	//}

	std::vector<osg::Vec3f> points_1;
	radar->getTestPoints(points_1);
	m_grid_points->clear();
	for (int i = 0; i < points_1.size(); i++)
	{
		m_grid_points->push_back(points_1[i]);
	}

}

osg::Vec4f RadarRender::getColorFromPd(double pd)
{
	const osg::Vec4f colors[4] = { osg::Vec4f(0, 0, 1, 0), osg::Vec4f(0, 1, 1, 0.4), osg::Vec4f(1, 1, 0, 0.6), osg::Vec4f(1, 0, 0, 1.0) };
	if (pd > 1.0)
		return colors[3];
	if (pd < 0.0)
		return colors[0];
	int colorindex;
	for (colorindex = 0; colorindex < 3; colorindex++)
	{
		if (pd > colors[colorindex + 1].w())
		{
			continue;
		}
		else {
			break;
		}
	}
	float mixeff = (pd - colors[colorindex].w()) / (colors[colorindex + 1].w() - colors[colorindex].w());
	return colors[colorindex] + (colors[colorindex + 1] - colors[colorindex]) * mixeff;;
}
