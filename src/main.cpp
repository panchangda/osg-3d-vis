#include <QtWidgets/QApplication>


#include <numbers>
#include <osgViewer/Viewer>
#include <osg/Texture3D>
#include <osg/Texture2D>
#include <osg/Point>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osg/ShapeDrawable>
#include <osgUtil/Tessellator>


#include "Util.h"
#include "StreamLineMainWindow.h"
#include "VelocityFieldGPU.h"
#include "Selection.h"
#include "streamline_cpu.h"
#include "VelocityLIC.h"
#include "main_window.h"
#include "OSGB.h"
#include "OSGPCDLoader.h"
#include "VolumeLIC.h"
#include "VortexDetect.h"
#include "Radar.h"
#include "Axis.h"
#include "DotChart.h"
#include "ColumnChart.h"
#include "chart.h"


// use high definition earth image
// #define USE_HD_EARTH_IMAGE


static inline osg::ref_ptr<osg::CoordinateSystemNode> createEarth() {
	auto* hints = new osg::TessellationHints;
	hints->setDetailRatio(5.0f);

	auto* sd = new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(0.0, 0.0, 0.0), osg::WGS_84_RADIUS_POLAR), hints);
	sd->setUseVertexBufferObjects(true);
	//sd->setUseVertexArrayObject(true);

	auto* geode = new osg::Geode;
	geode->addDrawable(sd);
	auto filename = std::string(OSG_3D_VIS_DATA_PREFIX) +
#ifdef USE_HD_EARTH_IMAGE
		"Land_shallow_topo_2048.jpg";
#else
		"1024px-Land_shallow_topo_2048.jpg";
#endif
		
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(
		0, new osg::Texture2D(osgDB::readImageFile(filename)));
;
	osg::ref_ptr<osg::CoordinateSystemNode> csn = new osg::CoordinateSystemNode;
	osg::EllipsoidModel* epmodel = new osg::EllipsoidModel();
	csn->setEllipsoidModel(epmodel);
	csn->addChild(geode);

	return csn;

}


osg::ref_ptr<osg::Geometry> createSphereGeometry(float radius, unsigned int rings, unsigned int sectors,  float i, float j)
{
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
    osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);

    float const R = 1.0 / (float)(rings - 1);
    float const S = 1.0 / (float)(sectors - 1);

    for (unsigned int r = 0; r < rings; ++r)
    {
        for (unsigned int s = 0; s < sectors; ++s)
        {
            float const y = std::sin(-M_PI_2 + M_PI * r * R);
            float const x = std::cos(2 * M_PI * s * S) * std::sin(M_PI * r * R);
            float const z = std::sin(2 * M_PI * s * S) * std::sin(M_PI * r * R);

            vertices->push_back(osg::Vec3(x * radius+i/2, y * radius+j/2, z * radius));
            normals->push_back(osg::Vec3(x, y, z));
        }
    }

    for (unsigned int r = 0; r < rings - 1; ++r)
    {
        for (unsigned int s = 0; s < sectors - 1; ++s)
        {
            indices->push_back(r * sectors + s);
            indices->push_back((r + 1) * sectors + (s + 1));
            indices->push_back(r * sectors + (s + 1));

            indices->push_back(r * sectors + s);
            indices->push_back((r + 1) * sectors + s);
            indices->push_back((r + 1) * sectors + (s + 1));
        }
    }

    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	geometry->setVertexAttribArray(0, vertices.get(), osg::Array::BIND_PER_VERTEX);
	geometry->setVertexAttribArray(1, normals, osg::Array::BIND_PER_VERTEX);

    geometry->addPrimitiveSet(indices);

    return geometry;
}


auto createLightModel(osg::ref_ptr<osg::Camera> camera ) {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	osg::ref_ptr<osg::Vec3Array> metalColors = new osg::Vec3Array;
	{
		// 金属颜色示例 (RGB 范围 0.0 - 1.0)
		osg::Vec3 aluminumColor(0.77f, 0.77f, 0.77f);   // 铝
		osg::Vec3 goldColor(1.0f, 0.84f, 0.0f);         // 金
		osg::Vec3 copperColor(0.72f, 0.45f, 0.2f);      // 铜
		osg::Vec3 silverColor(0.75f, 0.75f, 0.75f);     // 银
		osg::Vec3 ironColor(0.56f, 0.57f, 0.58f);       // 铁
		osg::Vec3 bronzeColor(0.55f, 0.47f, 0.14f);     // 青铜
		osg::Vec3 titaniumColor(0.54f, 0.57f, 0.62f);   // 钛
		osg::Vec3 platinumColor(0.81f, 0.78f, 0.75f);   // 铂

		// 添加颜色到数组
		metalColors->push_back(aluminumColor);
		metalColors->push_back(goldColor);
		metalColors->push_back(copperColor);
		metalColors->push_back(silverColor);
		metalColors->push_back(ironColor);
		metalColors->push_back(bronzeColor);
		metalColors->push_back(titaniumColor);
		metalColors->push_back(platinumColor);
	}

	osg::ref_ptr<osg::Vec3Array> metalF0Values = new osg::Vec3Array;

	{
		// 金属的菲涅耳 F0 值 (RGB 范围 0.0 - 1.0)
		osg::Vec3 aluminumF0(0.913f, 0.921f, 0.924f);  // 铝
		osg::Vec3 goldF0(1.000f, 0.765f, 0.336f);      // 金
		osg::Vec3 copperF0(0.955f, 0.637f, 0.538f);    // 铜
		osg::Vec3 silverF0(0.972f, 0.960f, 0.915f);    // 银
		osg::Vec3 ironF0(0.562f, 0.565f, 0.578f);      // 铁
		osg::Vec3 bronzeF0(0.495f, 0.424f, 0.275f);    // 青铜
		osg::Vec3 titaniumF0(0.542f, 0.497f, 0.449f);  // 钛
		osg::Vec3 platinumF0(0.673f, 0.637f, 0.585f);  // 铂

		// 添加 F0 值到数组
		metalF0Values->push_back(aluminumF0);
		metalF0Values->push_back(goldF0);
		metalF0Values->push_back(copperF0);
		metalF0Values->push_back(silverF0);
		metalF0Values->push_back(ironF0);
		metalF0Values->push_back(bronzeF0);
		metalF0Values->push_back(titaniumF0);
		metalF0Values->push_back(platinumF0);
	}

	// 创建一个 Vec3Array 并添加一些油漆颜色值
	osg::ref_ptr<osg::Vec3Array> paintColors = new osg::Vec3Array;
	{
		// 油漆颜色示例 (RGB 范围 0.0 - 1.0)
		osg::Vec3 redPaint(1.0f, 0.0f, 0.0f);       // 红色
		osg::Vec3 greenPaint(0.0f, 1.0f, 0.0f);     // 绿色
		osg::Vec3 bluePaint(0.0f, 0.0f, 1.0f);      // 蓝色
		osg::Vec3 whitePaint(1.0f, 1.0f, 1.0f);     // 白色
		osg::Vec3 blackPaint(0.0f, 0.0f, 0.0f);     // 黑色
		osg::Vec3 yellowPaint(1.0f, 1.0f, 0.0f);    // 黄色
		osg::Vec3 cyanPaint(0.0f, 1.0f, 1.0f);      // 青色
		osg::Vec3 magentaPaint(1.0f, 0.0f, 1.0f);   // 品红色

		// 添加颜色到数组
		paintColors->push_back(redPaint);
		paintColors->push_back(greenPaint);
		paintColors->push_back(bluePaint);
		paintColors->push_back(whitePaint);
		paintColors->push_back(blackPaint);
		paintColors->push_back(yellowPaint);
		paintColors->push_back(cyanPaint);
		paintColors->push_back(magentaPaint);
	}

	// 创建一个 Vec3Array 并添加对应的菲涅耳 F0 值
	osg::ref_ptr<osg::Vec3Array> paintF0Values = new osg::Vec3Array;

	{
		// 油漆的菲涅耳 F0 值 (假设接近非金属材料的典型反射率)
		osg::Vec3 redF0(0.04f, 0.04f, 0.04f);       // 红色
		osg::Vec3 greenF0(0.04f, 0.04f, 0.04f);     // 绿色
		osg::Vec3 blueF0(0.04f, 0.04f, 0.04f);      // 蓝色
		osg::Vec3 whiteF0(0.04f, 0.04f, 0.04f);     // 白色
		osg::Vec3 blackF0(0.04f, 0.04f, 0.04f);     // 黑色
		osg::Vec3 yellowF0(0.04f, 0.04f, 0.04f);    // 黄色
		osg::Vec3 cyanF0(0.04f, 0.04f, 0.04f);      // 青色
		osg::Vec3 magentaF0(0.04f, 0.04f, 0.04f);   // 品红色

		// 添加 F0 值到数组
		paintF0Values->push_back(redF0);
		paintF0Values->push_back(greenF0);
		paintF0Values->push_back(blueF0);
		paintF0Values->push_back(whiteF0);
		paintF0Values->push_back(blackF0);
		paintF0Values->push_back(yellowF0);
		paintF0Values->push_back(cyanF0);
		paintF0Values->push_back(magentaF0);
	}

	osg::Vec3 lightPositions[4] = {
	osg::Vec3(20.0f, 20.0f, 8.0f),
	osg::Vec3(-20.0f, 20.0f, 8.0f),
	osg::Vec3(20.0f, -20.0f, 8.0f),
	osg::Vec3(-20.0f, -20.0f, 8.0f)
	};

	osg::Vec3 lightColors[4] = {
		osg::Vec3(2200.0f, 2200.0f, 2200.0f),
		osg::Vec3(2200.0f, 2200.0f, 2200.0f),
		osg::Vec3(2200.0f, 2200.0f, 2200.0f),
		osg::Vec3(2200.0f, 2200.0f, 2200.0f)
	};


	for (int i = -2; i <= 2; ++i) {

		osg::ref_ptr<osg::Geometry> geometry = createSphereGeometry(0.2, 20, 20, i, 1);
		geode->addDrawable(geometry);

		auto stateset = geometry->getOrCreateStateSet();
		// 创建 Uniform 并设置初始值


		stateset->addUniform(new osg::Uniform("albedo", metalColors->at(i + 3)));
		stateset->addUniform(new osg::Uniform("F0", metalF0Values->at(i + 3)));
		stateset->addUniform(new osg::Uniform("roughness", float(0.33)));
		stateset->addUniform(new osg::Uniform("metallic", float(0.96)));
	}
	for (int i = -2; i <= 2; ++i) {

		osg::ref_ptr<osg::Geometry> geometry = createSphereGeometry(0.2, 20, 20, i, -1);
		geode->addDrawable(geometry);

		auto stateset = geometry->getOrCreateStateSet();
		// 创建 Uniform 并设置初始值


		stateset->addUniform(new osg::Uniform("albedo", paintColors->at(i + 3)));
		stateset->addUniform(new osg::Uniform("F0", paintF0Values->at(i + 3)));
		stateset->addUniform(new osg::Uniform("roughness", float(0)));
		stateset->addUniform(new osg::Uniform("metallic", float(0.0)));
	}


	// 创建着色器程序并设置到几何体的 StateSet
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet;
	// bind shader 
	osg::ref_ptr<osg::Shader> vertexShader = new osg::Shader(osg::Shader::VERTEX);
	osg::ref_ptr<osg::Shader> fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
	vertexShader->loadShaderSourceFromFile("../shader/lightvs.glsl");
	fragmentShader->loadShaderSourceFromFile("../shader/lightps.glsl");
	osg::ref_ptr<osg::Program> program1 = new osg::Program;
	program1->addShader(vertexShader.get());
	program1->addShader(fragmentShader.get());

	stateset->setAttributeAndModes(program1, osg::StateAttribute::ON);
	geode->setStateSet(stateset);

	stateset->addUniform(new osg::Uniform("normalMatrix", osg::Matrixf::identity()));
	stateset->addUniform(new osg::Uniform("model", osg::Matrixf::identity()));
	stateset->addUniform(new osg::Uniform("view", osg::Matrixf(camera->getViewMatrix())));
	stateset->addUniform(new osg::Uniform("projection", osg::Matrixf(camera->getProjectionMatrix())));

	stateset->addUniform(new osg::Uniform("ao", float(0.5)));
	osg::ref_ptr<osg::Uniform> lightColorsUniform = new osg::Uniform(osg::Uniform::Type::FLOAT_VEC3, "lightColors", 4);
	for (int i = 0; i < 4; ++i) {
		lightColorsUniform->setElement(i, lightColors[i]);
	}
	stateset->addUniform(lightColorsUniform.get());

	osg::ref_ptr<osg::Uniform> lightPositionsUniform = new osg::Uniform(osg::Uniform::Type::FLOAT_VEC3, "lightPositions", 4);
	for (int i = 0; i < 4; ++i) {
		lightPositionsUniform->setElement(i, lightPositions[i]);
	}
	stateset->addUniform(lightPositionsUniform.get());


	stateset->addUniform(new osg::Uniform("camPos", osg::Vec3(0, 0, 1)));


	return geode;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   
    
	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(300, 300, 1920, 1080);

	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	osg::ref_ptr<osg::Camera> camera = viewer.getCamera();
	osg::ref_ptr<osg::Group> root = new osg::Group();
	
	camera->setViewMatrixAsLookAt(osg::Vec3(0.0, 0, 1), osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(0.0, 1, 0.0));
	camera->setProjectionMatrixAsPerspective(45.0, 1.78, 0.1, 100.0);

	// create earth
	osg::ref_ptr<osg::CoordinateSystemNode> csn = createEarth();
	root->addChild(csn);

	auto chartWindow = new Chart(root, &viewer);
	chartWindow->SetUIButtonConnections();
	chartWindow->show();

	///* Show Main Window */
	auto mainWindow = new main_window();
	mainWindow->show();


	/* show light model */
	//auto geode = createLightModel(camera);
	//root->addChild(geode);


	///* Show Streamline: GPU */
	//auto mainWnd = new StreamLineMainWindow();
	//mainWnd->show();

	//auto slPtr = VelocityFieldGPU::Generate(
	//	root, camera,
	//	llhRange(-10.0, 52.0, 99.0, 150.0, 2000.0f, 2000.f));

	//mainWnd->SetStreamLine(slPtr);



	///* Show Streamline: CPU (enable choosing one streamling)*/
	//Selection::StreamLine* sl = new Selection::StreamLine(viewer, root,  camera.get(), llhRange(-10.0, 52.0, 99.0, 150.0, 1000.0f, 1000.f));
	//auto wnd = new streamline_cpu();
	//wnd->setStreamLineConnections(sl);
	//wnd->show();



	/* Show LIC */
	//VelocityLIC::Generate3D(root, camera.get(), llhRange(-10.f, 52.f, 99.f, 150.f, 100000.f, 1000000.f), 3);
	//osg::ref_ptr<osg::Geode> vlicGeode = VelocityLIC::Generate(root, camera.get(), llhRange(-10.f, 52.f, 99.f, 150.f, 100000.f, 200000.f));
	//root->addChild(vlicGeode);

	/* Show Radar */
	//Radar::initRadar(llhRange(25.f, 40.f, 100.f, 115.f, 1.f, 6000.f));
	//Radar::addRadar(llhRange(25.f, 35.f, 105.f, 115.f, 1.f, 6000.f));
	//Radar::addRadar(llhRange(28.f, 38.f, 102.f, 112.f, 1.f, 6000.f));
	//Radar::addRadar(llhRange(29.f, 39.f, 103.f, 113.f, 1.f, 6000.f));
	//Radar::ExportRadar();
	//Radar::submitRadar(root);

	/* Show Vortex Detect */
	//VortexDetect::Generate3D(root, camera.get(), llhRange(-10.f, 52.f, 99.f, 150.f, 100000.f, 1000000.f), 3);
	VortexDetect::GenerateML(root, camera.get(), llhRange(0.f, 30.f, 120.f, 150.f, 100000.f, 1000000.f));
	//osg::ref_ptr<osg::Geode> vortexGeode = VortexDetect::Generate(root, camera.get(), llhRange(-10.f, 52.f, 99.f, 150.f, 100000.f, 2000000.f));
	//root->addChild(vortexGeode);

	/* Show OSGB Loader */
	//osg::ref_ptr<osg::CoordinateSystemNode> osgbNode = OSGB::LoadFromPath(std::string(OSG_3D_VIS_DATA_PREFIX) + "QJXC");
	//root->addChild(osgbNode);



	/* Show Point Cloud Loader */
	//OSGPCDLoader* loader = new OSGPCDLoader();
	//loader->LoadFromFileXYZRGB(std::string(OSG_3D_VIS_DATA_PREFIX) + "milk_cartoon_all_small_clorox.pcd");
	//loader->LoadFromFileXYZRGB(std::string(OSG_3D_VIS_DATA_PREFIX) +"CSite1_orig-utm.pcd");
	//loader->LoadFromFolder(std::string(OSG_3D_VIS_DATA_PREFIX) +"kitchen");
	//loader->LoadFromFileXYZI(std::string(OSG_3D_VIS_DATA_PREFIX) +"Statues_4.pcd");
	//root->addChild(loader->geode);

	

	//DotChart* dot = new DotChart();
	//root->addChild(dot->generateDot());

	//ColumnChart* col = new ColumnChart();
	//root->addChild(col->generateColumn());
    






	viewer.setSceneData(root.get());
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.setLightingMode(osg::View::NO_LIGHT);
	viewer.realize();



	// calculate fps
	float prevTime = osg::Timer::instance()->time_s();
	int flag = 0;
	while (!viewer.done()) {

		a.processEvents();

		float currentTime = osg::Timer::instance()->time_s();
		//std::cout << "FPS: " << 1.0f / (currentTime - prevTime) << std::endl;
		prevTime = currentTime;

		viewer.frame();


		//viewer.setCameraManipulator(new osgGA::TrackballManipulator);

	/*if (flag == 0) {
			Axis* axis = new Axis();
			root->addChild(axis->DrawAxis(0, 0, 0, 21, 21, 21, 1));
			flag = 1;

			//viewer.setCameraManipulator(new osgGA::TrackballManipulator, true);
		}
		*/	

	}

	/*a.exec();*/

    return 0;
}
