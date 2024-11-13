#ifndef RADARUI_H
#define RADARUI_H

#include <QWidget>

#include "./Radar.h"

#include "./ui_radar.h"


class RadarUi : public QWidget
{
    Q_OBJECT

public:
	Ui::RadarUi* ui;
	Radar::Radar* rad;

    RadarUi(QWidget *parent = nullptr);
    ~RadarUi();
	void setConnect();
	void setRad(Radar::Radar* r) {
		rad = r;
		setConnect();
	}
	void updateR(double value) {
		rad->updateR(value);
	};
	void updateG(double value){
		rad->updateG(value);
	};
	void updateB(double value) {
		rad->updateB(value);
	};
	void updateA(double value) {
		rad->updateA(value);
	};
	void updateLineWidth(double value) {
		rad->updateLineWidth(value);
	};
	void updateDrawStyle(int index) {
		rad->updateDrawStyle(index);
	};

	void updateEMIR(double value)
	{
		rad->updateEMIR(value);
	}
	void updateEMIG(double value)
	{
		rad->updateEMIG(value);
	}
	void updateEMIB(double value)
	{
		rad->updateEMIB(value);
	}
	void updateEMIPHI(double value)
	{
		rad->updateEMIPHI(value);
	}
	void updateEMITHETA(double value)
	{
		rad->updateEMITHETA(value);
	}

};
#endif // RADARUI_H
