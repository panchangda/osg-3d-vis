// qt
#include <QtWidgets/QApplication>

// osg
#include <osgViewer/Viewer>
#include <osg/Point>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/FlightManipulator>
#include <osg/ShapeDrawable>
#include <osgUtil/Tessellator>


// self
#include "earth/Earth.hpp"
#include "particles/Particle.h"
#include "globals.h"
#include "util.h"
#include "charts/chart.h"
#include "charts/ColumnChart.h"
#include "charts/DotChart.h"
#include "velocity_field/StreamlineCPU.h"
#include "velocity_field/streamline_cpu.h"


namespace osg_3d_vis {
	float frameTime;
	float frameFPS;
}

osg::ref_ptr<osg::Group> loadScene(osgViewer::Viewer &viewer);
void initViewer(osgViewer::Viewer &viewer);
void prepareViewer(osgViewer::Viewer &viewer, const osg::ref_ptr<osg::Group>& root);
void draw( osgViewer::Viewer &viewer, const QApplication& QApp);

int main(int argc, char *argv[])
{
	// osg粒子效果源文件写死了加载"Images/xxx.rgb"，通过指定路径使其能够找到图像
	osgDB::Registry::instance()->getDataFilePathList().push_back(std::string(OSG_3D_VIS_DATA_PREFIX) + "OpenSceneGraph-Data");


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
// create earth
	if(osg_3d_vis::drawEarth) {
		auto earth = new osg_3d_vs::Earth(root);
	}



	// create particles:
	// explore, snow, rain...
	auto particle = new osg_3d_vs::Particle(root);

	// fog
	if(osg_3d_vis::enableFog) {
		auto fog = osg_3d_vs::Particle::createFog(osg_3d_vis::fogLinear);
		root->getOrCreateStateSet()->setAttributeAndModes(fog, osg::StateAttribute::ON);
	}



	///* Show Main Window */
	// auto mainWindow = new main_window();
	// mainWindow->show();


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



	/* Show Streamline: CPU (enable choosing one streamling) */
	auto streamlineCPU = new osg_3d_vis::StreamLineCPU(viewer, root,  viewer.getCamera(), osg_3d_vis::llhRange(-10.0, 52.0, 99.0, 150.0, 1000.0f, 1000.f));


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
	// VortexDetect::GenerateML(root, camera.get(), llhRange(0.f, 30.f, 120.f, 150.f, 100000.f, 1000000.f));
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


	/*
	 * charts are dynamically controlled by ui
	 *
	 */
	if(osg_3d_vis::showCharts) {
		auto chartWindow = new Chart(root, &viewer);
		chartWindow->SetUIButtonConnections();
		chartWindow->show();
	}
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


	camera->setViewMatrixAsLookAt(osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(1.0, 0, 0.0));
	camera->setProjectionMatrixAsPerspective(45.0, 1.78, 0.1, 100.0);

}
void prepareViewer(osgViewer::Viewer &viewer, const osg::ref_ptr<osg::Group>& root) {

	viewer.setSceneData(root.get());
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);

	viewer.setLightingMode(osg::View::NO_LIGHT);
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
		osg_3d_vis::frameFPS = 1.0f / (currentTime - prevTime);;

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