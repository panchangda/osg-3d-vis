#include "Radar.h"

#include <unordered_set>
#include <osg/BlendFunc>
#include <osg/BlendEquation>
#include <osg/BlendColor>

#include "cmath"
#include "../loader/OSGPCDLoader.h"

// ui part
// color and drawway

static osg::ref_ptr<osg::Uniform> colorUniform = new osg::Uniform("mainColor", osg::Vec4f(1,0.6,0.6,0.4));
static osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth(2);
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
	rt->getOrCreateStateSet()->getUniform("mainColor")->set(color);
};
void Radar::Radar::updateG(double value) {
	osg::Vec4 color;
	colorUniform->get(color);
	color.g() = value;
	rt->getOrCreateStateSet()->getUniform("mainColor")->set(color);
};
void Radar::Radar::updateB(double value) {
	osg::Vec4 color;
	colorUniform->get(color);
	color.b() = value;
	rt->getOrCreateStateSet()->getUniform("mainColor")->set(color);
};
void Radar::Radar::updateA(double value) {
	osg::Vec4 color;
	colorUniform->get(color);
	color.a() = value;
	rt->getOrCreateStateSet()->getUniform("mainColor")->set(color);

};
void Radar::Radar::updateLineWidth(double value) {
	lineWidth->setWidth(value);
};
void Radar::Radar::updateDrawStyle(int index) {
	if (index == 0) {
		osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
		rt->getOrCreateStateSet()->setAttribute(polyMode);
		rt->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::OFF);

	}
	else if( index == 1){
		osg::ref_ptr<osg::PolygonMode> polyMode = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		rt->getOrCreateStateSet()->setAttribute(polyMode);
		rt->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
	}
}

void Radar::Radar::updateEMI(int index)
{
	if( index == 0 )
	{
		rt->getOrCreateStateSet()->getUniform("isDisruption")->set(true);
	}else if( index == 1)
	{
		rt->getOrCreateStateSet()->getUniform("isDisruption")->set(false);
	}
}

Radar::Radar::Radar(osgViewer::Viewer& viewer, osg::ref_ptr<osg::Group> root)
{
	setCamera(viewer.getCamera());
	addFactor({ 30,100,600000,10000000 });
	addFactor({ 20,105,600000,10000000});
	addFactor({ 35,120,600000,12000000 });

	Addllh(osg_3d_vis::llhRange(25.l, 35., 105., 115., 1., 600000.));
	Addllh(osg_3d_vis::llhRange(14., 19., 90., 100., 100000., 600000.));

	Addllh(osg_3d_vis::llhRange(30., 32., 118., 120., 200000., 600000.));
	Addllh(osg_3d_vis::llhRange(37., 39., 100., 102., 2500., 600000.));
	Addllh(osg_3d_vis::llhRange(30., 39., 130., 140., 3000., 600000.));
	Addllh(osg_3d_vis::llhRange(14., 20., 95., 100., 1., 600000.));

	Addllh(osg_3d_vis::llhRange(10., 20., 110., 120., 200000., 600000.));
	Addllh(osg_3d_vis::llhRange(5., 10., 110., 120., 300000., 600000.));
	submit(viewer, root);
}

void Radar::Radar::submit(osgViewer::Viewer& viewer, osg::ref_ptr<osg::Group> root)
{

	rt = new osg::Geode;
	for (int i = 0; i < ranges.size(); ++i) {
		rt->addDrawable(Geos[i]);
	}

	osg::ref_ptr<osg::Shader> VertexShader = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> FragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
	VertexShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanVS.glsl");
	FragmentShader->loadShaderSourceFromFile(std::string(OSG_3D_VIS_SHADER_PREFIX) + "radar/RadarRoundScanPS.glsl");
	osg::ref_ptr<osg::Program> Program = new osg::Program;
	Program->addShader(VertexShader);
	Program->addShader(FragmentShader);




	osg::ref_ptr<osg::Uniform> factorArray = new osg::Uniform(osg::Uniform::Type::FLOAT_VEC4, "factor4", factors.size());
	for (int i = 0; i < factors.size(); ++i) {
		factorArray->setElement(i, factors[i]);
	}
	osg::ref_ptr<osg::Uniform> isDisruptionUniform = new osg::Uniform("isDisruption", false);

	osg::ref_ptr<osg::StateSet> StateSet = rt->getOrCreateStateSet();

	StateSet->setAttributeAndModes(Program);
	StateSet->addUniform(colorUniform);
	StateSet->addUniform(mvpUniform);
	StateSet->addUniform(isDisruptionUniform);

	StateSet->addUniform(factorArray);
	StateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	StateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	root->addChild(rt);
}

void Radar::Radar::addFactor(osg::Vec4 fac)
{
	double x, y, z;
	osg_3d_vis::llh2xyz_Ellipsoid(fac.x(), fac.y(), fac.z(), x, y, z);
	osg::Vec4 p(x, y, z, fac.w());
	factors.push_back(p);
}


osg::ref_ptr<osg::Geometry>  Radar::Radar::Generate(osg_3d_vis::llhRange range)
{
	const int levels = 10;
	std::vector<osg::Vec2> heights{ {0,0.3},{0.3,0.2}, { 0.2,0.5 },{0.5,0.4},{0.4,0.65},{0.65,0.55},{0.55,0.75},{0.75,0.65},{0.65,0.9},{0.9,0.75},{0.75,0.83},{0.83,0.79}
	};
	std::vector<osg::Vec2> radius{
		{0,1},{1,0.2}, { 0.2,0.9 },{0.9,0.4},{0.4,0.85},{0.85,0.3	},{0.3,0.8},{0.8,0.2},{0.2,0.7},{0.7,0.1},{0.1,0.3},{0.3,0}
	};
	std::vector<osg::Vec3> llhs;

	double lowLo = range.minLongtitude;
	double highLo = range.maxLongtitude;
	double lowLa = range.minLatitude;
	double highLa = range.maxLatitude;
	double lowH = range.minHeight;
	double highH = range.maxHeight;
	auto a = (range.maxLatitude - range.minLatitude) / 2;;
	auto b = (range.maxLongtitude - range.minLongtitude) / 2;
	auto c = range.maxHeight - range.minHeight;

	osg::ref_ptr<osg::Vec4Array> Vec4arrays = new osg::Vec4Array;

	osg::Vec3 center((lowLa + highLa) / 2, (lowLo + highLo) / 2, lowH);


	for (int level = 0; level < levels; level+=2) {
		for (double theta = 0; theta <= osg::PI / 2; theta += osg::PI / 40)
		{
			auto RadiusFactor =  radius[level].x() + (radius[level].y() - radius[level].x()) * sin(theta);
			auto HeightFactor = heights[level].x() + (heights[level].y() - heights[level].x()) * sin(theta);
			auto llh = osg::Vec3(osg::Vec2(a, b) * RadiusFactor, (float)c * HeightFactor);
			llhs.push_back(llh);
		}
		for (double theta = 0; theta <= osg::PI / 2; theta += osg::PI / 40)
		{
			auto RadiusFactor = radius[level+1].x() + (radius[level+1].y() - radius[level+1].x()) * (1- cos(theta));
			auto HeightFactor = heights[level+1].x() + (heights[level+1].y() - heights[level+1].x()) * (1 - cos(theta));
			auto llh = osg::Vec3(osg::Vec2(a, b) * RadiusFactor, (float)c * HeightFactor);
			llhs.push_back(llh);
		}
	}
	for(const auto& llh : llhs)
	{
		auto la = llh.x(), lo = llh.y(), he = llh.z();
		for(double theta = 0; theta <= 2*osg::PI; theta += 0.0174533 )
		{
			double k = center.x() + la * cos(theta);
			double j = center.y() + lo * sin(theta);
			double x, y, z;
			osg_3d_vis::llh2xyz_Ellipsoid(k, j, he, x, y, z);
			osg::Vec4 p(x, y, z, 1.);
			Vec4arrays->push_back(p);
		}
	} 

	const int maxk = Vec4arrays->size() / 360;
	const int cnt = 360;
	osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(GL_TRIANGLES);
	for (int cntk = 0; cntk < maxk-1; ++cntk) {
		int init = cntk * cnt;
		for (int i = init; i < init + cnt - 1; ++i)
		{
			// 第一个三角形 (i, i+1, i+n)
			indices->push_back(i);
			indices->push_back(i + 1);
			indices->push_back(i + cnt);

			// 第二个三角形 (i+1, i+1+n, i+n)
			indices->push_back(i + 1);
			indices->push_back(i + 1 + cnt);
			indices->push_back(i + cnt);
		}
		// 处理最后一个四边形（需要封闭圆环）
		int last = init + cnt - 1;  
		int first = init;           
		int upper_last = last + cnt; 
		int upper_first = first + cnt; 

		// 第一个三角形 (last, first, upper_last)
		indices->push_back(last);
		indices->push_back(first);
		indices->push_back(upper_last);

		// 第二个三角形 (first, upper_first, upper_last)
		indices->push_back(first);
		indices->push_back(upper_first);
		indices->push_back(upper_last);
	}

	osg::ref_ptr<osg::Geometry> ret = new osg::Geometry;
	ret->setVertexAttribArray(0, Vec4arrays, osg::Array::BIND_PER_VERTEX);
	ret->addPrimitiveSet(indices);
	return ret;
}
