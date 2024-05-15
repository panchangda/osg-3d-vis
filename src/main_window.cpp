#include "main_window.h"

#include "streamline_cpu.h"
#include "ui_main_window.h"

main_window::main_window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::main_window)
{
    ui->setupUi(this);
}

main_window::~main_window()
{
    delete ui;
}

void main_window::SetUIButtonConnections()
{

    connect(ui->StreamlineButton, &QPushButton::clicked, this, &main_window::createStreamLine);
    connect(ui->LICButton, &QPushButton::clicked, this, &main_window::createLIC);

}


void main_window::createStreamLine()
{
    /*Selection::StreamLine* sl = new Selection::StreamLine(viewer, root,  camera.get(), llhRange(-10.0, 52.0, 99.0, 150.0, 1000.0f, 1000.f));
	auto wnd = new streamline_cpu();
	wnd->setStreamLineConnections(sl);
	wnd->show();*/
}

void main_window::createLIC()
{

}
