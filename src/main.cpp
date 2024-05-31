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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

   
    
	osgViewer::Viewer viewer;
	viewer.setUpViewInWindow(300, 300, 1920, 1080);

	viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
	osg::ref_ptr<osg::Camera> camera = viewer.getCamera();
	osg::ref_ptr<osg::Group> root = new osg::Group();
	
	// create earth
	//osg::ref_ptr<osg::CoordinateSystemNode> csn = createEarth();
	//root->addChild(csn);

	auto chartWindow = new Chart(root, &viewer);
	chartWindow->SetUIButtonConnections();
	chartWindow->show();

	/* Show Main Window */
	//auto mainWindow = new main_window();
	//mainWindow->show();



	/* Show Streamline: GPU */
	//auto mainWnd = new StreamLineMainWindow();
	//mainWnd->show();

	//auto slPtr = VelocityFieldGPU::Generate(
	//	root, camera,
	//	llhRange(-10.0, 52.0, 99.0, 150.0, 2000.0f, 2000.f));

	//mainWnd->SetStreamLine(slPtr);



	/* Show Streamline: CPU (enable choosing one streamling)*/
	//Selection::StreamLine* sl = new Selection::StreamLine(viewer, root,  camera.get(), llhRange(-10.0, 52.0, 99.0, 150.0, 1000.0f, 1000.f));
	//auto wnd = new streamline_cpu();
	//wnd->setStreamLineConnections(sl);
	//wnd->show();



	/* Show LIC */
	/*osg::ref_ptr<osg::Geode> vlicGeode = VelocityLIC::Generate(root, camera.get(), llhRange(-10.f, 52.f, 99.f, 150.f, 100000.f, 200000.f));
	root->addChild(vlicGeode);*/

	/* Show Radar */

	//Radar::initRadar(llhRange(25.f, 40.f, 100.f, 115.f, 1.f, 6000.f));
	//Radar::addRadar(llhRange(25.f, 35.f, 105.f, 115.f, 1.f, 6000.f));
	//Radar::addRadar(llhRange(28.f, 38.f, 102.f, 112.f, 1.f, 6000.f));
	//Radar::addRadar(llhRange(29.f, 39.f, 103.f, 113.f, 1.f, 6000.f));
	//Radar::submitRadar(root);

	/* Show Vortex Detect */
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
