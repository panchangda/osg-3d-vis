#pragma once

#include <QtWidgets/QMainWindow>

#include <ui_streamline_cpu.h>

#include "StreamlineCPU.h"


class streamline_cpu : public QWidget
{
    Q_OBJECT

public:
    streamline_cpu(QWidget* parent = nullptr):QWidget(parent) {
        ui.setupUi(this);
    }
    /*
    0. arrows style (r g b a adjustment, follow line color(selected included)) 
    1. line style (dotted line...), line color template/ rgba adjustment
    2. selection style (r g b a adjustment)
    3. random seed (random, density based, informationEntropy based)
    4. smooth (quadratic bezier, arcSpline)
    5. color template (rainbow, full black/white,  others)
    6. line width, decay rate, line length, h, speed scale factor...
    */
    void setStreamLineConnections(osg_3d_vis::StreamLineCPU* _sl) {
        sl = _sl;

        /* lines attribute */
        connect(ui.horizontalSlider_LineWidth, &QSlider::valueChanged, this, &streamline_cpu::updateLineWidth);
        connect(ui.horizontalSlider_FadeOpacity, &QSlider::valueChanged, this, &streamline_cpu::updateFadeOpacity);
        connect(ui.horizontalSlider_SpeedScaleFactor, &QSlider::sliderReleased, this, &streamline_cpu::updateSpeedScaleFactor);
        connect(ui.horizontalSlider_H, &QSlider::sliderReleased, this, &streamline_cpu::updateH);
        connect(ui.horizontalSlider_PointDensity, &QSlider::sliderReleased, this, &streamline_cpu::updatePointDensity);
        /* lines styles */
        connect(ui.comboBox_LineStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &streamline_cpu::updateLineStyle);
        /* lines colors*/
        connect(ui.comboBox_ColorTemplate, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &streamline_cpu::updateLineColorTemplate);
        connect(ui.horizontalSlider_HueOffset, &QSlider::sliderReleased, this, &streamline_cpu::updateHueOffset);
        connect(ui.horizontalSlider_HueRange, &QSlider::sliderReleased, this, &streamline_cpu::updateHueRange);
        connect(ui.horizontalSlider_Saturation, &QSlider::sliderReleased, this, &streamline_cpu::updateSaturation);
        connect(ui.horizontalSlider_Value, &QSlider::sliderReleased, this, &streamline_cpu::updateValue);
        connect(ui.checkBox_UseSingleColor, &QCheckBox::stateChanged, this, &streamline_cpu::updateLineUseSameColor);
        connect(ui.horizontalSlider_LineR, &QSlider::valueChanged, this, &streamline_cpu::updateLineR);
        connect(ui.horizontalSlider_LineG, &QSlider::valueChanged, this, &streamline_cpu::updateLineG);
        connect(ui.horizontalSlider_LineB, &QSlider::valueChanged, this, &streamline_cpu::updateLineB);
        connect(ui.horizontalSlider_LineA, &QSlider::valueChanged, this, &streamline_cpu::updateLineA);
        /* selection colors */
        connect(ui.horizontalSlider_SelectionR, &QSlider::valueChanged, this, &streamline_cpu::updateSelectionR);
        connect(ui.horizontalSlider_SelectionG, &QSlider::valueChanged, this, &streamline_cpu::updateSelectionG);
        connect(ui.horizontalSlider_SelectionB, &QSlider::valueChanged, this, &streamline_cpu::updateSelectionB);
        connect(ui.horizontalSlider_SelectionA, &QSlider::valueChanged, this, &streamline_cpu::updateSelectionA);
        /* arrows */ 
        connect(ui.checkBox_ShowArrow, &QCheckBox::stateChanged, this, &streamline_cpu::updateArrowVisibility);
        connect(ui.checkBox_UseLineColor, &QCheckBox::stateChanged, this, &streamline_cpu::updateArrowUseLineColor);
        connect(ui.horizontalSlider_ArrowR, &QSlider::valueChanged, this, &streamline_cpu::updateArrowR);
        connect(ui.horizontalSlider_ArrowG, &QSlider::valueChanged, this, &streamline_cpu::updateArrowG);
        connect(ui.horizontalSlider_ArrowB, &QSlider::valueChanged, this, &streamline_cpu::updateArrowB);
        connect(ui.horizontalSlider_ArrowA, &QSlider::valueChanged, this, &streamline_cpu::updateArrowA);
        /* random seed */
        connect(ui.comboBox_RandomSeedSelection, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &streamline_cpu::updateRandomSeedSelection);
        /* smooth */
        connect(ui.comboBox_Smooth, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &streamline_cpu::updateSmoothAlgorithm);
        /* differential method*/
        connect(ui.comboBox_DifferentialMethod, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &streamline_cpu::updateDifferentialMethod);
    }
    ~streamline_cpu() {}
private slots:
    void updateLineWidth(int value) {
        sl->updateLineWidth(value);
    }
    void updateFadeOpacity(int value) {
        sl->updateFadeOpacity(value);
    }
    void updateSpeedScaleFactor() {
        sl->updateSpeedScaleFactor(ui.horizontalSlider_SpeedScaleFactor->value());
    }
    void updateH() {
        sl->updateH(ui.horizontalSlider_H->value());
    }
    void updatePointDensity() {
        sl->updatePointDensity(ui.horizontalSlider_PointDensity->value());
    }
    void updateLineStyle(int value) {
        sl->updateLineStyle(value);
    }
    void updateHueOffset() {
        sl->updateHueOffset(ui.horizontalSlider_HueOffset->value());
    }
    void updateHueRange() {
        sl->updateHueRange(ui.horizontalSlider_HueRange->value());
    }
    void updateSaturation() {
        sl->updateSaturation(ui.horizontalSlider_Saturation->value());
    }
    void updateValue(){    
        sl->updateValue(ui.horizontalSlider_Value->value());
    }

    void updateLineColorTemplate(int value) {
        sl->updateLineColorTemplate(value);
    }
    void updateLineUseSameColor(int value) {
        sl->updateLineUseSameColor(value);
    }
    void updateLineR(int value) {
        sl->updateLineR(value);
    }
    void updateLineG(int value) {
        sl->updateLineG(value);
    }
    void updateLineB(int value) {
        sl->updateLineB(value);
    }
    void updateLineA(int value) {
        sl->updateLineA(value);
    }
    void updateSelectionR(int value) {
        sl->updateSelectionR(value);
    }
    void updateSelectionG(int value) {
        sl->updateSelectionG(value);
    }
    void updateSelectionB(int value) {
        sl->updateSelectionB(value);
    }
    void updateSelectionA(int value) {
        sl->updateSelectionA(value);
    }
    void updateArrowVisibility(int value) {
        sl->updateArrowVisibility(value);
    }
    void updateArrowUseLineColor(int value) {
        sl->updateArrowUseLineColor(value);
    }
    void updateArrowR(int value) {
        sl->updateArrowR(value);
    }
    void updateArrowG(int value) {
        sl->updateArrowG(value);
    }
    void updateArrowB(int value) {
        sl->updateArrowB(value);
    }
    void updateArrowA(int value) {
        sl->updateArrowA(value);
    }
    void updateRandomSeedSelection(int value) {
        sl->updateRandomSeedSelection(value);
    }
    void updateSmoothAlgorithm(int value) {
        sl->updateSmoothAlgorithm(value);
    }
    void updateDifferentialMethod(int value) {
        sl->updateDifferentialMethod(value);
    }
private:
    Ui::streamline_cpuClass ui;
    osg_3d_vis::StreamLineCPU* sl;
};
