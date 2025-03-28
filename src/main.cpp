// qt headers
#include <QtWidgets/QApplication>

// osg headers
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Point>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgUtil/Tessellator>


// osg-3d-vis headers
#include "earth/Earth.hpp"
#include "particles/Particle.h"
#include "globals.h"
#include "util.h"
#include "charts/chart.h"
#include "charts/ColumnChart.h"
#include "charts/DotChart.h"
#include "earth/Cloud.h"
#include "earth/Grass.h"
#include "earth/Tree.h"
#include "loader/OSGB.h"
#include "radar/Radar.h"
#include "velocity_field/StreamlineCPU.h"
#include "velocity_field/StreamlineGPU.h"
#include "radar/radarui.h"
#include "pbr/pbr.hpp"
#include "velocity_field/rttcamera.h"
#include "earth/Forest.h"

osg::ref_ptr<osg::Group> loadScene(osgViewer::Viewer &viewer);
void osgSetUp();
void initViewer(osgViewer::Viewer &viewer);
void prepareViewer(osgViewer::Viewer &viewer, const osg::ref_ptr<osg::Group>& root);
void draw( osgViewer::Viewer &viewer, const QApplication& QApp);
Radar::Radar* meshRadar = nullptr;

int main(int argc, char *argv[])
{
	osgSetUp();

	// Qt
    QApplication QApp(argc, argv);

	// init viewer
	osgViewer::Viewer viewer;
	initViewer(viewer);

	// load scene
	osg::ref_ptr<osg::Group> root = loadScene(viewer);

	// prepare viewer for draw
	prepareViewer(viewer, root);

	// render loop
	draw(viewer, QApp);

    return 0;
}

osg::ref_ptr<osg::Group> loadScene(osgViewer::Viewer &viewer) {
	osg::ref_ptr<osg::Group> root = new osg::Group();

	/*
	 *  Earth Elliposoid
	 */
	if(osg_3d_vis::drawEarth) {
		auto earth = new osg_3d_vis::Earth(root);
	}

	/*
	 * Particle Effects:
	 * create particles: explode, snow, rain...
	 */
	auto particle = new osg_3d_vis::Particle(root);

	// fog
	if(osg_3d_vis::enableFog) {
		auto fog = osg_3d_vis::Particle::createFog(osg_3d_vis::fogLinear);
		root->getOrCreateStateSet()->setAttributeAndModes(fog, osg::StateAttribute::ON);
	}

	///*
	// * GPU Instances
	// */
	// if( osg_3d_vis::drawGrass)
	// {
		// auto grass = new osg_3d_vis::Grass(root, viewer.getCamera());
	// }
	//
	// if( osg_3d_vis::drawTree)
	// {
	// 	auto tree = new osg_3d_vis::Tree(root, viewer.getCamera());
	//
	// }

//	auto forest = new osg_3d_vis::Forest(root, viewer.getCamera());

	/*
	 *Cloud
	 */

	if( osg_3d_vis::showCloud)
	{
		auto cloud = new Cloud(root);
	}


	///*
	// * Velocity Field Visualizations
	// */
	// Show Streamline: CPU (enable choosing one streamling)
	//
	 auto streamlineCPU = new osg_3d_vis::StreamLineCPU(
	 	viewer,
	 	root,
	 	viewer.getCamera(),
	 	osg_3d_vis::llhRange(-10.0, 52.0, 99.0, 150.0, 1000.0f, 1000.f));


	//// Show Streamline: GPU
	// auto slPtr = VelocityFieldGPU::Generate(
	// 	root,
	// 	viewer.getCamera(),
	// 	osg_3d_vis::llhRange(-10.0, 52.0, 99.0, 150.0, 2000.0f, 2000.f));


	// Show LIC
	//VelocityLIC::Generate3D(root, camera.get(), llhRange(-10.f, 52.f, 99.f, 150.f, 100000.f, 1000000.f), 3);
	//osg::ref_ptr<osg::Geode> vlicGeode = VelocityLIC::Generate(root, camera.get(), llhRange(-10.f, 52.f, 99.f, 150.f, 100000.f, 200000.f));
	//root->addChild(vlicGeode);

	// Show Vortex Detect
	//VortexDetect::Generate3D(root, camera.get(), llhRange(-10.f, 52.f, 99.f, 150.f, 100000.f, 1000000.f), 3);
	// VortexDetect::GenerateML(root, camera.get(), llhRange(0.f, 30.f, 120.f, 150.f, 100000.f, 1000000.f));
	//osg::ref_ptr<osg::Geode> vortexGeode = VortexDetect::Generate(root, camera.get(), llhRange(-10.f, 52.f, 99.f, 150.f, 100000.f, 2000000.f));
	//root->addChild(vortexGeode);




	/////*
	//// * PBR
	//// */
	///i
	if (osg_3d_vis::pbr) {
		PBR* pbr = new PBR();
		pbr->createLightModel(root, viewer.getCamera());
	}


	///*
	// * VoxelRadar
	// */
	//VoxelRadar::initRadar(osg_3d_vis::llhRange(25.f, 40.f, 100.f, 115.f, 1.f, 6000.f));
	//VoxelRadar::addRadar(osg_3d_vis::llhRange(25.f, 35.f, 105.f, 115.f, 1.f, 6000.f));
	//VoxelRadar::addRadar(osg_3d_vis::llhRange(28.f, 38.f, 102.f, 112.f, 1.f, 6000.f));
	//VoxelRadar::addRadar(osg_3d_vis::llhRange(29.f, 39.f, 103.f, 113.f, 1.f, 6000.f));
	//VoxelRadar::ExportRadar();
	//VoxelRadar::submitRadar(root);


	///*
	// * Mesh radar
	// */


	// RadarUi* rui = new RadarUi();
	// meshRadar = new Radar::Radar(viewer, root);
	// rui->setRad(meshRadar);
	// rui->show();


	/*
	 * Loader Examples
	 */

	// Show OSGB Loader
	// osg::ref_ptr<osg::CoordinateSystemNode> osgbNode = OSGBLoader::LoadFromPath(std::string(OSG_3D_VIS_DATA_PREFIX) + "QJXC");
	// root->addChild(osgbNode);

	// Show Point Cloud Loader
	//OSGPCDLoader* loader = new OSGPCDLoader();
	//loader->LoadFromFileXYZRGB(std::string(OSG_3D_VIS_DATA_PREFIX) + "milk_cartoon_all_small_clorox.pcd");
	//loader->LoadFromFileXYZRGB(std::string(OSG_3D_VIS_DATA_PREFIX) +"CSite1_orig-utm.pcd");
	//loader->LoadFromFolder(std::string(OSG_3D_VIS_DATA_PREFIX) +"kitchen");
	//loader->LoadFromFileXYZI(std::string(OSG_3D_VIS_DATA_PREFIX) +"Statues_4.pcd");
	//root->addChild(loader->geode);


	///*
	// * 3D Charts: charts are dynamically controlled by ui
	// */
	if(osg_3d_vis::showCharts) {
		auto chartWindow = new Chart(root, &viewer);
		chartWindow->SetUIButtonConnections();
		chartWindow->show();
	}



	// For RTT Test
	// osg_3d_vis::rttcamera* testRTTCamera = new osg_3d_vis::rttcamera(viewer);
	// root->addChild(testRTTCamera->cameraNode);
	// root->addChild(testRTTCamera->fullscreenQuadGeode);

	return root.get();
}
/*
 * Note:
 *	Camera must be set before streamlineCPU initialization, otherwise it will draw nothing
 *  Fuck!
 */


void initViewer(osgViewer::Viewer &viewer) {
	viewer.setUpViewInWindow(300, 300, 1920, 1080);

	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	osg::ref_ptr<osg::Camera> camera = viewer.getCamera();

	camera->setViewMatrixAsLookAt(osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(0.0, 0.0, -1), osg::Vec3(0, 1, 0.0));
	camera->setProjectionMatrixAsPerspective(60.0, 1.78, 0.001, 10000.0);

	// 创建轨迹球操纵器
	osg::ref_ptr<osgGA::TrackballManipulator> manipulator = new osgGA::TrackballManipulator;
	if(osg_3d_vis::drawEarth &&
		(osg_3d_vis::bSetCameraToCity || osg_3d_vis::pbr || osg_3d_vis::drawTree || osg_3d_vis::drawGrass || osg_3d_vis::drawExplode || osg_3d_vis::drawWeather)
		) {
		const osg_3d_vis::CameraViewParams& camParams = osg_3d_vis::cameraCityMap.at(osg_3d_vis::cameraTargetCity);
		// 设置摄像机的“Home”位置
		manipulator->setHomePosition(camParams.position, camParams.center, camParams.up);

		// set moving cameraPosition
		osg_3d_vis::cameraPosition = camParams.position;
		osg_3d_vis::cameraCenter = camParams.center;
	}


	viewer.setCameraManipulator(manipulator);
}

void prepareViewer(osgViewer::Viewer &viewer, const osg::ref_ptr<osg::Group>& root) {

    osgUtil::Optimizer optimizer;
    optimizer.optimize(root.get());

	viewer.setSceneData(root.get());

	viewer.setLightingMode(osg_3d_vis::lightingMode);

	viewer.addEventHandler(new osgViewer::StatsHandler);//查看帧数 s

	viewer.realize();
}

void draw( osgViewer::Viewer &viewer, const QApplication& QApp) {

	// calculate fps
	float prevTime = osg::Timer::instance()->time_s();
	int flag = 0;
	while (!viewer.done()) {

		QApplication::processEvents();

		float currentTime = osg::Timer::instance()->time_s();
		osg_3d_vis::frameTime =  (currentTime - prevTime);
		osg_3d_vis::frameFPS = 1.0f / (currentTime - prevTime);

		prevTime = currentTime;
		viewer.frame();

		/*if (flag == 0) {
				Axis* axis = new Axis();
				root->addChild(axis->DrawAxis(0, 0, 0, 21, 21, 21, 1));
				flag = 1;

				//viewer.setCameraManipulator(new osgGA::TrackballManipulator, true);
			}
			*/

	}

	/*a.exec();*/
}

void osgSetUp() {
	//
	// 设置提示信息详略等级
	osg::setNotifyLevel(osg_3d_vis::osgNotifyLevel);
	// osg粒子效果源文件写死了加载"Images/xxx.rgb"，通过指定路径使其能够找到图像
	osgDB::Registry::instance()->getDataFilePathList().push_back(std::string(OSG_3D_VIS_DATA_PREFIX) + "OpenSceneGraph-Data");
}