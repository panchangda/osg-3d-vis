#pragma once

#include <string>
#include <fstream>
#include <set>
#include <map>

#include "Concave.h"
#define M_PI 3.1415926536

const double log_10 = log(10.0);
const double pow_4_pi_3 = pow(4 * M_PI, 3);

struct ApmRadarPara {
	double Pt;
	double G;
	double area;
	double randa;
	double K;
	double Ts;
	double Bn;
	double F0;
	double N;
	double Pfa;
public:
	ApmRadarPara();
	~ApmRadarPara();
	void UpdatePara();
	void UpdateDValue();
	double GetAeff()
	{
		return Aeff;
	}
	double GetBeff()
	{
		return Beff;
	}
	void MakeRGridFromFile(const std::string fn);
	void getTestPoints(std::vector<osg::Vec3f>& points);

	void getIsoLineConcave(std::vector<osg::Vec2f>& line, double pd, float concavity);
private:
	double Aeff, Beff;
	float* D_At_Points;
	osg::Vec3f* Loss_AtPoints;
	int dcount, hcount;
	float dmax, hmax;
	float dstep, hstep;
	concave* _concave;
	float edge_length;


	float getRadarDFromLFR(double l, double f, double r);
	float getDFromPd(double pd);
	void _makePointSet(std::vector<Vec2_X>& pointsset, float curD);

public:

	void SetMaxAngle_length_box(float a, float l, float maxb, float minb);

	double m_curd;
};


class RadarRender {
public:
	/*ptr<Program> m_pRadarGridPointsRender;
	ptr<Program> m_pRadarRoundScanRender;
	ptr<Program> m_pRadarRenderLine;

	ptr<Program> m_pRadarRenderWithPlane;
	ptr<Program> m_pClipPlaneRender;

	ptr<Program> m_pRenderWithColor;*/

public:
	RadarRender();
	~RadarRender();
	//void renderRadar3d(ptr<FrameBuffer> fb, const mat4f& mvp);
	void GenerateIsoMesh(double pd, bool isround = false, bool isSymmetryX = true);
	osg::Vec4f getColorFromPd(double pd);
	void SetNewPd(double pd)
	{
		m_curpd = pd;
		GenerateIsoMesh(m_curpd, true, true);
	}
	double getPd()
	{
		return m_curpd;
	}
	void SetNewRadar(const std::string& fn)
	{
		radar->MakeRGridFromFile(fn);
	}
	ApmRadarPara* radar;

public:

	osg::ref_ptr<osg::Vec4Array> m_lines_normals = new osg::Vec4Array();
	//ptr<Mesh<vec3f, unsigned int>> m_grid_points;
	osg::ref_ptr<osg::Vec3Array> m_grid_points = new osg::Vec3Array();
	//ptr<Mesh<vec3f, unsigned short>> m_x_line;
	//ptr<Mesh<vec3f, unsigned short>> m_x_point;
	//ptr<Mesh<vec3f, unsigned short>> m_y_line;
	//ptr<Mesh<vec3f, unsigned short>> m_y_point;
	//ptr<Mesh<vec3f, unsigned short>> m_z_line;
	//ptr<Mesh<vec3f, unsigned short>> m_z_point;
	//ptr<Mesh<vec4f, unsigned short>> m_lines_normals_round;
	//ptr<Mesh<vec4f, unsigned short>> m_Quad;
	double m_curpd;

public:
	bool pointFlag, lineFlag, surfaceFlag;

	//void SaveRadaData(const std::string& fn);

private:
	//void MakeAxisGeometry();
};