#ifndef RADARUI_H
#define RADARUI_H

#include <QWidget>

#include "Radar.h"

#include "ui_radar.h"

class RadarUi : public QWidget
{
    Q_OBJECT

public:
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
	void updateEMI(int index)
	{
		rad->updateEMI(index);
	};
private:
    Ui::RadarUi *ui;
	Radar::Radar* rad;
};
#endif // RADARUI_H
