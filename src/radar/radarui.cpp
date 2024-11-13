#include "radarui.h"
#include "ui_radar.h"

RadarUi::RadarUi(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RadarUi)
{
    ui->setupUi(this);
}

RadarUi::~RadarUi()
{
    delete ui;
}
void RadarUi::setConnect() {
	connect(ui->ColorR, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RadarUi::updateR);
	connect(ui->ColorG, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RadarUi::updateG);
	connect(ui->ColorB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RadarUi::updateB);
	connect(ui->ColorA, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RadarUi::updateA);
	connect(ui->lineWidth, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RadarUi::updateLineWidth);
	connect(ui->DisplayList, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &RadarUi::updateDrawStyle);
	connect(ui->EMIR, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RadarUi::updateEMIR);
	connect(ui->EMIG, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RadarUi::updateEMIG);
	connect(ui->EMIB, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RadarUi::updateEMIB);
	connect(ui->EMIPHI, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RadarUi::updateEMIPHI);
	connect(ui->EMITHETA, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &RadarUi::updateEMITHETA);
}

