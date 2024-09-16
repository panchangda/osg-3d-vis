#pragma once

#include <QtWidgets/QMainWindow>

#include <ui_streamline.h>

#include "StreamlineGPU.h"

namespace Ui
{
	class StreamLineMainWindow;
}

class StreamLineMainWindow : public QWidget {
	Q_OBJECT

private:
	VelocityFieldGPU::StreamLineGPU* sl;
	Ui::StreamLineMainWindow ui;
public:
	StreamLineMainWindow(QWidget* parent = nullptr) : QWidget(parent)
	{
		ui.setupUi(this);
		
	}

	void SetStreamLine(VelocityFieldGPU::StreamLineGPU* _sl){
		sl = _sl;

		connect(ui.horizontalSlider_SpeedFactor,SIGNAL(valueChanged(int)), 
			this, SLOT(updateSpeedFactor(int)));
		connect(ui.horizontalSlider_DropRate,SIGNAL(valueChanged(int)), 
			this, SLOT(updateDropRate(int)));
		connect(ui.horizontalSlider_LineWidth,SIGNAL(valueChanged(int)), 
			this, SLOT(updateLineWidth(int)));
		connect(ui.horizontalSlider_DecayRate,SIGNAL(valueChanged(int)), 
			this, SLOT(updateDecayRate(int)));
		connect(ui.comboBox_RandomSeedSelection,SIGNAL(currentIndexChanged(int)),
			this, SLOT(updateRandomSeedSelection(int)));
		connect(ui.comboBox_Smooth,SIGNAL(currentIndexChanged(int)),
			this, SLOT(updateSmooth(int)));
		connect(ui.comboBox_ColorTemplate,SIGNAL(currentIndexChanged(int)),
			this, SLOT(updateColorTemplate(int)));

	}
private slots:
	void updateSpeedFactor(int value){
		sl->updateSpeedFactor(value);
	}
	void updateDropRate(int value){
		sl->updateDropRate(value);
	}
	void updateLineWidth(int value){
		sl->updateLineWidth(value);
	}
	void updateDecayRate(int value){
		sl->updateDecayRate(value);
	}
	void updateRandomSeedSelection(int index){
		sl->updateRandomSeedSelection(index);
	}
	void updateSmooth(int index){
		sl->updateSmooth(index);
	}
	void updateColorTemplate(int index){
		sl->updateColorTemplate(index);
	}

};