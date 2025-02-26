#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>

#include<stack>
using namespace std;

namespace Ui {
class Chart;
}

class Chart : public QWidget
{
    Q_OBJECT

public:
    explicit Chart(QWidget *parent = nullptr);
    osg::ref_ptr<osg::Group>  root;
    osg::ref_ptr<osgViewer::Viewer> viewer;
    stack<unsigned int> index;
    

    ~Chart();
    Chart(osg::ref_ptr<osg::Group> r, osgViewer::Viewer * v);
    void SetUIButtonConnections();

private:
    Ui::Chart *ui;
    void createAxis();
    void createColumn();
    void createDot();
    void createPolyline();
    void createCurve();
    void createPie();
    void createParallel();
    void createRadar();
    void exit()
    {
        std::exit(0);
    }
};

#endif // CHART_H
