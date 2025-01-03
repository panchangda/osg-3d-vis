//
// Created by pcd on 2025/1/3.
//

#ifndef FOCUS_H
#define FOCUS_H

#include <QWidget>
#include "./ui_focus.h"
#include <osg/Camera>
#include "../globals.h"

class focus : public QWidget {
Q_OBJECT

public:

    osgViewer::Viewer* viewer;
    explicit focus(osgViewer::Viewer & v, QWidget *parent = nullptr) : QWidget(parent), ui(new Ui::focus), viewer(&v) {
        ui->setupUi(this);
        setConnect();
    };
    ~focus() override;
    void setConnect();

    void setCameraPosToEarth();
private:
    Ui::focus *ui;
};

inline void focus::setConnect() {
    connect(ui->checkBox, &QCheckBox::toggled, this, &focus::setCameraPosToEarth);
}

inline void focus::setCameraPosToEarth() {
    auto eye = osg::Vec3d(osg_3d_vis::HangZhouPos.x()+100000.0f, osg_3d_vis::HangZhouPos.y()+100000.0f, osg_3d_vis::HangZhouPos.z()+100000.0f);
    viewer->getCamera()->setViewMatrixAsLookAt(eye, osg_3d_vis::HangZhouPos, osg::Vec3d(0,0,1));
    viewer->frame();
}


#endif //FOCUS_H
