#include "chart.h"
#include "ui_chart.h"
#include "Axis.h"
#include "ColumnChart.h"
#include "DotChart.h"
#include "PolylineChart.h"
#include "CurveChart.h"
#include "PieChart.h"

Chart::Chart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chart)
{
    ui->setupUi(this);
}

void Chart::SetUIButtonConnections()
{

   connect(ui->ButtonAxis, &QPushButton::clicked, this, &Chart::createAxis);
   connect(ui->ButtonColumn, &QPushButton::clicked, this, &Chart::createColumn);
   connect(ui->ButtonDot, &QPushButton::clicked, this, &Chart::createDot);
   connect(ui->ButtonPolyline, &QPushButton::clicked, this, &Chart::createPolyline);
   connect(ui->ButtonCurve, &QPushButton::clicked, this, &Chart::createCurve);
   connect(ui->ButtonPie, &QPushButton::clicked, this, &Chart::createPie);
}

void Chart::createAxis()
{
    Axis* axis = new Axis();
    root->addChild(axis->DrawAxis(0, 0, 0, 21, 21, 21, 1));
    //viewer->
    viewer->setCameraManipulator(new osgGA::TrackballManipulator);
}

void Chart::createColumn()
{
    
    while (!index.empty()) {
        unsigned int top = index.top();
        root->removeChild(top);
        index.pop();
   }
    ColumnChart* col = new ColumnChart();
    osg::ref_ptr<osg::Geode> geo = col->generateColumn();
    root->addChild(geo);
    unsigned int idx = root->getChildIndex(geo);
    index.push(idx);
    viewer->setCameraManipulator(new osgGA::TrackballManipulator);
}

void Chart::createDot()
{
    /*DotChart* dot = new DotChart();
    root->addChild(dot->generateDot());
    viewer->setCameraManipulator(new osgGA::TrackballManipulator);*/

    while (!index.empty()) {
        unsigned int top = index.top();
        root->removeChild(top);
        index.pop();
    }
    DotChart* dot = new DotChart();
    osg::ref_ptr<osg::Geode> geo = dot->generateDot();
    root->addChild(geo);
    unsigned int idx = root->getChildIndex(geo);
    index.push(idx);
    viewer->setCameraManipulator(new osgGA::TrackballManipulator);

}

void Chart::createPolyline()
{
   /* PolylineChart* Polyline = new PolylineChart();
    root->addChild(Polyline->generateLineChart());
    viewer->setCameraManipulator(new osgGA::TrackballManipulator);*/

    while (!index.empty()) {
        unsigned int top = index.top();
        root->removeChild(top);
        index.pop();
    }
    PolylineChart* Polyline = new PolylineChart();
    osg::ref_ptr<osg::Geode> geo = Polyline->generateLineChart();
    root->addChild(geo);
    unsigned int idx = root->getChildIndex(geo);
    index.push(idx);
    viewer->setCameraManipulator(new osgGA::TrackballManipulator);
}

void Chart::createCurve()
{

    while (!index.empty()) {
        unsigned int top = index.top();
        root->removeChild(top);
        index.pop();
    }
    CurveChart* Curve = new CurveChart();
    osg::ref_ptr<osg::Geode> geo = Curve->generateCurveChart();
    root->addChild(geo);
    unsigned int idx = root->getChildIndex(geo);
    index.push(idx);
    viewer->setCameraManipulator(new osgGA::TrackballManipulator);
}

void Chart::createPie()
{

    while (!index.empty()) {
        unsigned int top = index.top();
        root->removeChild(top);
        index.pop();
    }
    PieChart* pie = new PieChart();
    osg::ref_ptr<osg::Geode> geo = pie->generatePieChart();
    root->addChild(geo);
    unsigned int idx = root->getChildIndex(geo);
    index.push(idx);
    viewer->setCameraManipulator(new osgGA::TrackballManipulator);
}


Chart::~Chart()
{
    delete ui;
}

Chart::Chart(osg::ref_ptr<osg::Group> r, osgViewer::Viewer *v)
{
    root = r;
    viewer = v;
    ui = new Ui::Chart;
    ui->setupUi(this);
}

