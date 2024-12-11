#include "Radar.h"

#include <qvariant.h>
#include <random>
#include <stdarg.h>
#include <osg/LineWidth>

// ui part
// color and drawway

static osg::ref_ptr<osg::Uniform> colorUniform = new osg::Uniform("mainColor", osg::Vec4f(1,0.6,0.6,0.4));
static osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth(2);
static osg::ref_ptr<osg::Uniform> moveSpeed = new osg::Uniform("phiThetaSpeed", osg::Vec2f(0.1, 0.05));
static osg::ref_ptr<osg::Uniform> moveTime = new osg::Uniform("t",0.0f);
static osg::ref_ptr<osg::Uniform> EmicolorUniform = new osg::Uniform("mainColor", osg::Vec4f(127 / 255, 1, 112 / 255,0.8));
enum DrawWay
{
	surface,
	line
};
static DrawWay usePattern = surface ;

void Radar::Radar::updateR(double value) {
	osg::Vec4 color;
	colorUniform->get(color);
	color.r() = value;
	RadarRT->getOrCreateStateSet()->getUniform("mainColor")->set(color);
};
void Radar::Radar::updateG(double value) {
	osg::Vec4 color;
	colorUniform->get(color);
	color.g() = value;
	RadarRT->getOrCreateStateSet()->getUniform("mainColor")->set(color);
};
void Radar::Radar::updateB(double value) {
	osg::Vec4 color;
	colorUniform->get(color);
	color.b() = value;
	RadarRT->getOrCreateStateSet()->getUniform("mainColor")->set(color);
};
void Radar::Radar::updateA(double value) {
	osg::Vec4 color;
	colorUniform->get(color);
	color.a() = value;
	RadarRT->getOrCreateStateSet()->getUniform("mainColor")->set(color);

};
void Radar::Radar::updateLineWidth(double value) {
	lineWidth->setWidth(value);
};
void Radar::Radar::updateDrawStyle(int index) {
	if (index == 0) {
		osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
		RadarRT->getOrCreateStateSet()->setAttribute(polyMode);
		RadarRT->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::OFF);

	}
	else if( index == 1){
		osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		RadarRT->getOrCreateStateSet()->setAttribute(polyMode);
		RadarRT->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
	}
}
void Radar::Radar::updateEMIR(double value)
{
	osg::Vec4 color;
	EmicolorUniform->get(color);
	color.r() = value;
	Emirt->getOrCreateStateSet()->getUniform("mainColor")->set(color);
}
void Radar::Radar::updateEMIG(double value)
{
	osg::Vec4 color;
	EmicolorUniform->get(color);
	color.g() = value;
	Emirt->getOrCreateStateSet()->getUniform("mainColor")->set(color);
}
void Radar::Radar::updateEMIB(double value)
{
	osg::Vec4 color;
	EmicolorUniform->get(color);
	color.b() = value;
	Emirt->getOrCreateStateSet()->getUniform("mainColor")->set(color);
}
void Radar::Radar::updateEMIPHI(double value)
{
	osg::Vec2 speed;
	moveSpeed->get(speed);
	speed.x() = value;
	Emirt->getOrCreateStateSet()->getUniform("phiThetaSpeed")->set(speed);

}
void Radar::Radar::updateEMITHETA(double value)
{
	osg::Vec2 speed;
	moveSpeed->get(speed);
	speed.y() = value;
	Emirt->getOrCreateStateSet()->getUniform("phiThetaSpeed")->set(speed);
}

osg::ref_ptr<osg::Geometry> createCircle(osg::Vec2 center, osg::Vec2 radius, unsigned int numSegments = 360) {
	osg::ref_ptr<osg::Geometry> circleGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_LINE_STRIP, 0);
	radius *= 0.9;

	// 计算圆周上的点
	for (unsigned int i = 0; i < numSegments; ++i) {
		float angle = 2.0f * osg::PI * float(i) / float(numSegments);
		auto la = center.x() + radius.x() * cos(angle), lo = center.y() + radius.y() * sin(angle);
		double x, y, z;
		osg_3d_vis::llh2xyz_Ellipsoid(la, lo, 60000, x, y, z);
		vertices->push_back(osg::Vec3(x, y, z));
		indices->push_back(i);
	}
	indices->push_back(0);

	circleGeometry->setVertexAttribArray(0, vertices, osg::Array::BIND_PER_VERTEX);
	circleGeometry->addPrimitiveSet(indices.get());
	return circleGeometry;
}

osg::ref_ptr<osg::Geometry> MakeCircle(osg::Vec2 center, osg::Vec2 radius, unsigned int numSegments = 360) {
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

	osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES, 0);

	float angleStep = 2.0f * osg::PI / numSegments;
	double xx, yy, zz;
	osg_3d_vis::llh2xyz_Ellipsoid({ center.x(), center.y(), 60000 }, xx, yy, zz);
	vertices->push_back(osg::Vec3(xx, yy, zz));
	osg_3d_vis::llh2xyz_Ellipsoid({ center.x(), center.y(), 500 }, xx, yy, zz);
	vertices->push_back(osg::Vec3(xx, yy, zz));
	float zTop = 60000;
	float zBottom = 500;

	for (unsigned int i = 0; i < numSegments; ++i) {
		float angle = angleStep * i;
		float x = radius.x() * cos(angle);
		float y = radius.y() * sin(angle);

		osg_3d_vis::llh2xyz_Ellipsoid(center.x() + x*0.9, center.y() + y*0.9, zTop, xx, yy, zz);
		vertices->push_back(osg::Vec3(xx, yy, zz));
		osg_3d_vis::llh2xyz_Ellipsoid(center.x() + x, center.y() + y, zBottom, xx, yy, zz);
		vertices->push_back(osg::Vec3(xx, yy, zz));
	}

	unsigned int topCenterIndex = 0;
	unsigned int bottomCenterIndex = 1;
	int mod = numSegments * 2 + 2;
	for (unsigned int i = 2; i < numSegments * 2; i += 2) {
		unsigned int next = (i + 2) % mod;
		indices->push_back(topCenterIndex);
		indices->push_back(i);
		indices->push_back(next);
	}
	indices->push_back(topCenterIndex);
	indices->push_back(mod - 2);
	indices->push_back(2);
	for (unsigned int i = 3; i < numSegments * 2; i += 2) {
		unsigned int next = (i + 2) % (numSegments * 2 + 2);
		indices->push_back(bottomCenterIndex);
		indices->push_back(i);
		indices->push_back(next);
	}
	indices->push_back(bottomCenterIndex);
	indices->push_back(mod - 1);
	indices->push_back(3);

	for (unsigned int i = 2; i < numSegments * 2; i += 2) {
		unsigned int next = (i + 2) % (numSegments * 2);
		indices->push_back(i);
		indices->push_back(i + 1);
		indices->push_back(next + 1);

		indices->push_back(i);
		indices->push_back(next + 1);
		indices->push_back(next);
	}
	unsigned int lastTop = numSegments * 2;
	unsigned int lastBottom = numSegments * 2 + 1;
	indices->push_back(lastTop);
	indices->push_back(lastBottom);
	indices->push_back(3);

	indices->push_back(lastBottom);
	indices->push_back(2);
	indices->push_back(3);

	geometry->setVertexArray(vertices);

	geometry->addPrimitiveSet(indices.get());

	return geometry;
}

osg::ref_ptr<osg::Geometry> MakeCircleSearch(osg::Vec2 center, osg::Vec2 radius, float height= 61110, unsigned int numSegments = 360) {
	osg::ref_ptr<osg::Geometry> circleGeometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;

	double x, y, z;
	osg_3d_vis::llh2xyz_Ellipsoid(center.x(), center.y(), height, x, y, z);
	vertices->push_back(osg::Vec3(x, y, z));
	osg::Vec3 point(x, y, z);
	radius *= 0.9;
	for (unsigned int i = 0; i < numSegments; ++i) {
		float angle = 2.0f * osg::PI * float(i) / float(numSegments);
		auto la = center.x() + radius.x() * cos(angle), lo = center.y() + radius.y() * sin(angle);

		osg_3d_vis::llh2xyz_Ellipsoid(la, lo, height, x, y, z);
		vertices->push_back(osg::Vec3(x, y, z));
	}

	osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);
	static std::random_device rd; 
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<> dis(1, numSegments);
	int k = dis(gen);
	for(int i= k; i<=k+30; ++i)
	{
		indices->push_back(0);
		indices->push_back(i%(numSegments+1));
		indices->push_back((i + 1) % (numSegments + 1));
	}
	circleGeometry->setVertexAttribArray(0, vertices, osg::Array::BIND_PER_VERTEX);
	circleGeometry->addPrimitiveSet(indices.get());
	circleGeometry->setVertexArray(vertices);

	return circleGeometry;
}

osg::ref_ptr<osg::Geode> createCustomCylinder(const osg::Vec3& center, float radius, float height, unsigned int numSegments = 360) {
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

	osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES, 0);

	float angleStep = 2.0f * osg::PI / numSegments;
	osg::Vec3 topCenter(center.x(), center.y(), center.z() + height / 2.0f); 
	osg::Vec3 bottomCenter(center.x(), center.y(), center.z() - height / 2.0f);
	double xx, yy, zz;
	osg_3d_vis::llh2xyz_Ellipsoid(topCenter, xx, yy, zz);
	vertices->push_back( osg::Vec3(xx,yy,zz));
	osg_3d_vis::llh2xyz_Ellipsoid(bottomCenter, xx, yy, zz);
	vertices->push_back(osg::Vec3(xx, yy, zz));


	for (unsigned int i = 0; i < numSegments; ++i) {
		float angle = angleStep * i;
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		float zTop = topCenter.z();
		float zBottom = bottomCenter.z();
		osg_3d_vis::llh2xyz_Ellipsoid(center.x()+x, center.y()+y, zTop, xx, yy, zz);
		vertices->push_back(osg::Vec3(xx, yy, zz));
		osg_3d_vis::llh2xyz_Ellipsoid(center.x() + x*0.99, center.y() + y*0.99, zBottom, xx, yy, zz);
		vertices->push_back(osg::Vec3(xx, yy, zz));
	}

	unsigned int topCenterIndex = 0;
	unsigned int bottomCenterIndex = 1;
	int mod = numSegments * 2 + 2;
	for (unsigned int i = 2; i < numSegments * 2; i += 2) {
		unsigned int next = (i + 2) % mod;
		indices->push_back(topCenterIndex);   
		indices->push_back(i);               
		indices->push_back(next);            
	}
	indices->push_back(topCenterIndex);
	indices->push_back(mod-2);
	indices->push_back(2);
	for (unsigned int i = 3; i < numSegments * 2; i += 2) {
		unsigned int next = (i + 2) % (numSegments * 2 + 2);
		indices->push_back(bottomCenterIndex);   
		indices->push_back(i );             
		indices->push_back(next);            
	}
	indices->push_back(bottomCenterIndex);
	indices->push_back(mod-1);
	indices->push_back(3);

	for (unsigned int i = 2; i < numSegments * 2; i += 2) {
		unsigned int next = (i + 2) % (numSegments * 2);
		indices->push_back(i);                   
		indices->push_back(i + 1);               
		indices->push_back(next + 1);            

		indices->push_back(i);                   
		indices->push_back(next + 1);            
		indices->push_back(next);                
	}
	unsigned int lastTop = numSegments * 2 ;
	unsigned int lastBottom = numSegments * 2 + 1;
	indices->push_back(lastTop);
	indices->push_back(lastBottom);
	indices->push_back(3);               

	indices->push_back(lastBottom);
	indices->push_back(2);        
	indices->push_back(3);               
	geometry->setVertexArray(vertices);

	geometry->addPrimitiveSet(indices.get());

	osg::ref_ptr<osg::Geode> geode = new osg::Geode();
	geode->addDrawable(geometry.get());

	return geode;
}
Radar::Radar::Radar(osgViewer::Viewer& viewer, osg::ref_ptr<osg::Group> root)
{
	setCamera(viewer.getCamera());


	Addllh(osg_3d_vis::llhRange(25.l, 35., 105., 115.));
	Addllh(osg_3d_vis::llhRange(14., 19., 90., 100.));

	Addllh(osg_3d_vis::llhRange(37., 39., 100., 102.));
	Addllh(osg_3d_vis::llhRange(30., 39., 130., 140.));
	Addllh(osg_3d_vis::llhRange(14., 20., 95., 100.));

	Addllh(osg_3d_vis::llhRange(10., 20., 110., 120.));
	Addllh(osg_3d_vis::llhRange(5., 10., 110., 120.));
	GenerateRadarMesh();
	GenerateRadarlineMesh();
	GenerateRadarsearchMesh();
	root->addChild(RadarRT);
	root->addChild(Radarline);
	root->addChild(Radarsearch);

	addEmi({ 16,115,60000,5 });
	GenerateEmiMesh();
	root->addChild(Emirt);
}

void Radar::Radar::GenerateRadarMesh()
{

	RadarRT = new osg::Geode;
	for (int i = 0; i < Circle.size(); ++i) {
		RadarRT->addChild(Circle[i]);
	}

	osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
	VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanVS.glsl");
	FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanPS.glsl");
	osg::ref_ptr<osg::Program> Program = new osg::Program;
	Program->addShader(VertexShader);
	Program->addShader(FragmentShader);
	osg::ref_ptr<osg::StateSet> StateSet = RadarRT->getOrCreateStateSet();

	StateSet->setAttributeAndModes(Program);
	StateSet->addUniform(colorUniform);
	StateSet->addUniform(mvpUniform);

	StateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	StateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

}

void Radar::Radar::GenerateRadarlineMesh()
{
	Radarline = new osg::Geode;
	for (int i = 0; i < CircleLine.size(); ++i) {
		Radarline->addChild(CircleLine[i]);
	}

	osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
	VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanVS.glsl");
	FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanPS.glsl");
	osg::ref_ptr<osg::Program> Program = new osg::Program;
	Program->addShader(VertexShader);
	Program->addShader(FragmentShader);
	osg::ref_ptr<osg::StateSet> StateSet = Radarline->getOrCreateStateSet();

	StateSet->setAttributeAndModes(Program);
	StateSet->addUniform(new osg::Uniform("mainColor", osg::Vec4(1,1,1,1)));
	StateSet->addUniform(mvpUniform);

	StateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	StateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	StateSet->setAttribute(polyMode);
	StateSet->setAttributeAndModes(new osg::LineWidth(0.8), osg::StateAttribute::ON);
}

void Radar::Radar::GenerateRadarsearchMesh()
{
	Radarsearch = new osg::Geode;
	for (int i = 0; i < Circlesearch.size(); ++i) {
		Radarsearch->addChild(Circlesearch[i]);
	}

	osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
	VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanVS.glsl");
	FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanPS.glsl");
	osg::ref_ptr<osg::Program> Program = new osg::Program;
	Program->addShader(VertexShader);
	Program->addShader(FragmentShader);
	osg::ref_ptr<osg::StateSet> StateSet = Radarsearch->getOrCreateStateSet();

	StateSet->setAttributeAndModes(Program);
	StateSet->addUniform(new osg::Uniform("mainColor", osg::Vec4(0.5, 1, 0, 0.6)));
	StateSet->addUniform(mvpUniform);

	StateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	StateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

}

void Radar::Radar::GenerateEmiMesh()
{
	Emirt = new osg::Geode;
	for (int i = 0; i < EmiGeos.size(); ++i) {
		Emirt->addChild(EmiGeos[i]);
	}

	osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
	VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/EmiVS.glsl");
	FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/EmiPS.glsl");
	osg::ref_ptr<osg::Program> Program = new osg::Program;
	Program->addShader(VertexShader);
	Program->addShader(FragmentShader);
	osg::ref_ptr<osg::StateSet> StateSet = Emirt->getOrCreateStateSet();

	StateSet->setAttributeAndModes(Program);
	StateSet->addUniform(EmicolorUniform);
	StateSet->addUniform(mvpUniform);
	StateSet->addUniform(moveSpeed);
	StateSet->addUniform(moveTime);
	moveTime->setUpdateCallback(new osg_3d_vis::TimeUniformCallback());
	StateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	StateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	StateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
}

osg::ref_ptr<osg::Geode> Radar::Radar::GenerateEmi(osg_3d_vis::llhRange range)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	osg::ref_ptr<osg::Geode> Geode = new osg::Geode;
	auto [minLa, maxLa, minLo, maxLo, minH, maxH] = range;
	osg::Vec2 center = osg::Vec2(minLa , minLo );
	osg::Vec2 radiusMAX = osg::Vec2(maxLa - minLa, maxLo - minLo) ;

	geode->addChild(createCustomCylinder(osg::Vec3(center, minH + 50000), radiusMAX.x(), 40000));
	
	geode->addChild(createCustomCylinder(osg::Vec3(center, minH + 100000), radiusMAX.x()/10, 60000));
	return geode;

}



osg::ref_ptr<osg::Geode>  Radar::Radar::Generate(osg_3d_vis::llhRange range)
{
	osg::ref_ptr<osg::Geode> Geode = new osg::Geode;
	auto [minLa, maxLa, minLo, maxLo, minH, maxH] = range;
	osg::Vec2 center = osg::Vec2(minLa + maxLa, minLo + maxLo) / 2;
	osg::Vec2 radiusMAX = osg::Vec2(maxLa - minLa, maxLo - minLo) / 2;


	Geode->addChild(MakeCircle(center, radiusMAX));

	return Geode;
}

osg::ref_ptr<osg::Geode> Radar::Radar::GenerateCiecleline(osg_3d_vis::llhRange range)
{
	osg::ref_ptr<osg::Geode> Geode = new osg::Geode;
	auto [minLa, maxLa, minLo, maxLo, minH, maxH] = range;
	osg::Vec2 center = osg::Vec2(minLa + maxLa, minLo + maxLo) / 2;
	osg::Vec2 radiusMAX = osg::Vec2(maxLa - minLa, maxLo - minLo) / 2;
	const int level = 5;


	// circle line part
	for (int i = 1; i <= level; ++i)
	{
		auto r = radiusMAX * (float)i / level;
		Geode->addChild(createCircle(center, r));
	}

	return Geode;
}

osg::ref_ptr<osg::Geode> Radar::Radar::GenerateCieclesearch(osg_3d_vis::llhRange range)
{
	osg::ref_ptr<osg::Geode> Geode = new osg::Geode;
	auto [minLa, maxLa, minLo, maxLo, minH, maxH] = range;
	osg::Vec2 center = osg::Vec2(minLa + maxLa, minLo + maxLo) / 2;
	osg::Vec2 radiusMAX = osg::Vec2(maxLa - minLa, maxLo - minLo) / 2;

	Geode->addChild(MakeCircleSearch(center, radiusMAX));

	return Geode;
}


