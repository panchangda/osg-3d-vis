/********************************************************************************
** Form generated from reading UI file 'streamline_cpu.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STREAMLINE_CPU_H
#define UI_STREAMLINE_CPU_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_streamline_cpuClass
{
public:
    QGridLayout *gridLayout_5;
    QSplitter *splitter;
    QLabel *label_4;
    QWidget *widget;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_24;
    QComboBox *comboBox_ColorTemplate;
    QWidget *widget1;
    QVBoxLayout *verticalLayout_9;
    QLabel *label_30;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_26;
    QSlider *horizontalSlider_HueRange;
    QHBoxLayout *horizontalLayout_24;
    QLabel *label_27;
    QSlider *horizontalSlider_HueOffset;
    QHBoxLayout *horizontalLayout_25;
    QLabel *label_28;
    QSlider *horizontalSlider_Saturation;
    QHBoxLayout *horizontalLayout_26;
    QLabel *label_29;
    QSlider *horizontalSlider_Value;
    QWidget *widget2;
    QVBoxLayout *verticalLayout_8;
    QLabel *label_31;
    QCheckBox *checkBox_UseSingleColor;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_20;
    QLabel *label_20;
    QSlider *horizontalSlider_LineR;
    QHBoxLayout *horizontalLayout_21;
    QLabel *label_21;
    QSlider *horizontalSlider_LineG;
    QHBoxLayout *horizontalLayout_22;
    QLabel *label_22;
    QSlider *horizontalSlider_LineB;
    QHBoxLayout *horizontalLayout_23;
    QLabel *label_23;
    QSlider *horizontalSlider_LineA;
    QHBoxLayout *horizontalLayout_27;
    QLabel *label_5;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *checkBox_UseLineColor;
    QCheckBox *checkBox_ShowArrow;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_6;
    QSlider *horizontalSlider_ArrowR;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QSlider *horizontalSlider_ArrowG;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_8;
    QSlider *horizontalSlider_ArrowB;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_9;
    QSlider *horizontalSlider_ArrowA;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_16;
    QSlider *horizontalSlider_SelectionR;
    QHBoxLayout *horizontalLayout_17;
    QLabel *label_17;
    QSlider *horizontalSlider_SelectionG;
    QHBoxLayout *horizontalLayout_18;
    QLabel *label_18;
    QSlider *horizontalSlider_SelectionB;
    QHBoxLayout *horizontalLayout_19;
    QLabel *label_19;
    QSlider *horizontalSlider_SelectionA;
    QHBoxLayout *horizontalLayout_28;
    QVBoxLayout *verticalLayout_6;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSlider *horizontalSlider_LineWidth;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_15;
    QSlider *horizontalSlider_FadeOpacity;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_13;
    QSlider *horizontalSlider_SpeedScaleFactor;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_12;
    QSlider *horizontalSlider_H;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_14;
    QSlider *horizontalSlider_PointDensity;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *comboBox_LineStyle;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_25;
    QComboBox *comboBox_DifferentialMethod;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_11;
    QComboBox *comboBox_Smooth;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_10;
    QComboBox *comboBox_RandomSeedSelection;

    void setupUi(QWidget *streamline_cpuClass)
    {
        if (streamline_cpuClass->objectName().isEmpty())
            streamline_cpuClass->setObjectName(QString::fromUtf8("streamline_cpuClass"));
        streamline_cpuClass->resize(621, 554);
        gridLayout_5 = new QGridLayout(streamline_cpuClass);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        splitter = new QSplitter(streamline_cpuClass);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        label_4 = new QLabel(splitter);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        splitter->addWidget(label_4);
        widget = new QWidget(splitter);
        widget->setObjectName(QString::fromUtf8("widget"));
        verticalLayout_5 = new QVBoxLayout(widget);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        label_24 = new QLabel(widget);
        label_24->setObjectName(QString::fromUtf8("label_24"));

        verticalLayout_5->addWidget(label_24);

        comboBox_ColorTemplate = new QComboBox(widget);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../data/rainbow.jpg"), QSize(), QIcon::Normal, QIcon::Off);
        comboBox_ColorTemplate->addItem(icon, QString());
        comboBox_ColorTemplate->setObjectName(QString::fromUtf8("comboBox_ColorTemplate"));

        verticalLayout_5->addWidget(comboBox_ColorTemplate);

        splitter->addWidget(widget);
        widget1 = new QWidget(splitter);
        widget1->setObjectName(QString::fromUtf8("widget1"));
        verticalLayout_9 = new QVBoxLayout(widget1);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        label_30 = new QLabel(widget1);
        label_30->setObjectName(QString::fromUtf8("label_30"));
        label_30->setAlignment(Qt::AlignCenter);
        label_30->setWordWrap(false);

        verticalLayout_9->addWidget(label_30);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        label_26 = new QLabel(widget1);
        label_26->setObjectName(QString::fromUtf8("label_26"));

        horizontalLayout_9->addWidget(label_26);

        horizontalSlider_HueRange = new QSlider(widget1);
        horizontalSlider_HueRange->setObjectName(QString::fromUtf8("horizontalSlider_HueRange"));
        horizontalSlider_HueRange->setMaximum(360);
        horizontalSlider_HueRange->setSliderPosition(360);
        horizontalSlider_HueRange->setOrientation(Qt::Horizontal);

        horizontalLayout_9->addWidget(horizontalSlider_HueRange);


        verticalLayout_9->addLayout(horizontalLayout_9);

        horizontalLayout_24 = new QHBoxLayout();
        horizontalLayout_24->setObjectName(QString::fromUtf8("horizontalLayout_24"));
        label_27 = new QLabel(widget1);
        label_27->setObjectName(QString::fromUtf8("label_27"));

        horizontalLayout_24->addWidget(label_27);

        horizontalSlider_HueOffset = new QSlider(widget1);
        horizontalSlider_HueOffset->setObjectName(QString::fromUtf8("horizontalSlider_HueOffset"));
        QFont font;
        font.setItalic(false);
        horizontalSlider_HueOffset->setFont(font);
        horizontalSlider_HueOffset->setAutoFillBackground(false);
        horizontalSlider_HueOffset->setMaximum(360);
        horizontalSlider_HueOffset->setOrientation(Qt::Horizontal);

        horizontalLayout_24->addWidget(horizontalSlider_HueOffset);


        verticalLayout_9->addLayout(horizontalLayout_24);

        horizontalLayout_25 = new QHBoxLayout();
        horizontalLayout_25->setObjectName(QString::fromUtf8("horizontalLayout_25"));
        label_28 = new QLabel(widget1);
        label_28->setObjectName(QString::fromUtf8("label_28"));

        horizontalLayout_25->addWidget(label_28);

        horizontalSlider_Saturation = new QSlider(widget1);
        horizontalSlider_Saturation->setObjectName(QString::fromUtf8("horizontalSlider_Saturation"));
        horizontalSlider_Saturation->setSliderPosition(75);
        horizontalSlider_Saturation->setOrientation(Qt::Horizontal);

        horizontalLayout_25->addWidget(horizontalSlider_Saturation);


        verticalLayout_9->addLayout(horizontalLayout_25);

        horizontalLayout_26 = new QHBoxLayout();
        horizontalLayout_26->setObjectName(QString::fromUtf8("horizontalLayout_26"));
        label_29 = new QLabel(widget1);
        label_29->setObjectName(QString::fromUtf8("label_29"));

        horizontalLayout_26->addWidget(label_29);

        horizontalSlider_Value = new QSlider(widget1);
        horizontalSlider_Value->setObjectName(QString::fromUtf8("horizontalSlider_Value"));
        horizontalSlider_Value->setSliderPosition(99);
        horizontalSlider_Value->setOrientation(Qt::Horizontal);

        horizontalLayout_26->addWidget(horizontalSlider_Value);


        verticalLayout_9->addLayout(horizontalLayout_26);

        splitter->addWidget(widget1);
        widget2 = new QWidget(splitter);
        widget2->setObjectName(QString::fromUtf8("widget2"));
        verticalLayout_8 = new QVBoxLayout(widget2);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        label_31 = new QLabel(widget2);
        label_31->setObjectName(QString::fromUtf8("label_31"));
        label_31->setAlignment(Qt::AlignCenter);

        verticalLayout_8->addWidget(label_31);

        checkBox_UseSingleColor = new QCheckBox(widget2);
        checkBox_UseSingleColor->setObjectName(QString::fromUtf8("checkBox_UseSingleColor"));

        verticalLayout_8->addWidget(checkBox_UseSingleColor);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setObjectName(QString::fromUtf8("horizontalLayout_20"));
        label_20 = new QLabel(widget2);
        label_20->setObjectName(QString::fromUtf8("label_20"));

        horizontalLayout_20->addWidget(label_20);

        horizontalSlider_LineR = new QSlider(widget2);
        horizontalSlider_LineR->setObjectName(QString::fromUtf8("horizontalSlider_LineR"));
        horizontalSlider_LineR->setMaximum(255);
        horizontalSlider_LineR->setSliderPosition(255);
        horizontalSlider_LineR->setOrientation(Qt::Horizontal);

        horizontalLayout_20->addWidget(horizontalSlider_LineR);


        verticalLayout_4->addLayout(horizontalLayout_20);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setObjectName(QString::fromUtf8("horizontalLayout_21"));
        label_21 = new QLabel(widget2);
        label_21->setObjectName(QString::fromUtf8("label_21"));

        horizontalLayout_21->addWidget(label_21);

        horizontalSlider_LineG = new QSlider(widget2);
        horizontalSlider_LineG->setObjectName(QString::fromUtf8("horizontalSlider_LineG"));
        horizontalSlider_LineG->setMaximum(255);
        horizontalSlider_LineG->setSliderPosition(255);
        horizontalSlider_LineG->setOrientation(Qt::Horizontal);

        horizontalLayout_21->addWidget(horizontalSlider_LineG);


        verticalLayout_4->addLayout(horizontalLayout_21);

        horizontalLayout_22 = new QHBoxLayout();
        horizontalLayout_22->setObjectName(QString::fromUtf8("horizontalLayout_22"));
        label_22 = new QLabel(widget2);
        label_22->setObjectName(QString::fromUtf8("label_22"));

        horizontalLayout_22->addWidget(label_22);

        horizontalSlider_LineB = new QSlider(widget2);
        horizontalSlider_LineB->setObjectName(QString::fromUtf8("horizontalSlider_LineB"));
        horizontalSlider_LineB->setMaximum(255);
        horizontalSlider_LineB->setSliderPosition(255);
        horizontalSlider_LineB->setOrientation(Qt::Horizontal);

        horizontalLayout_22->addWidget(horizontalSlider_LineB);


        verticalLayout_4->addLayout(horizontalLayout_22);

        horizontalLayout_23 = new QHBoxLayout();
        horizontalLayout_23->setObjectName(QString::fromUtf8("horizontalLayout_23"));
        label_23 = new QLabel(widget2);
        label_23->setObjectName(QString::fromUtf8("label_23"));

        horizontalLayout_23->addWidget(label_23);

        horizontalSlider_LineA = new QSlider(widget2);
        horizontalSlider_LineA->setObjectName(QString::fromUtf8("horizontalSlider_LineA"));
        horizontalSlider_LineA->setMaximum(255);
        horizontalSlider_LineA->setSliderPosition(255);
        horizontalSlider_LineA->setOrientation(Qt::Horizontal);

        horizontalLayout_23->addWidget(horizontalSlider_LineA);


        verticalLayout_4->addLayout(horizontalLayout_23);


        verticalLayout_8->addLayout(verticalLayout_4);

        splitter->addWidget(widget2);

        gridLayout_5->addWidget(splitter, 0, 0, 1, 2);

        horizontalLayout_27 = new QHBoxLayout();
        horizontalLayout_27->setObjectName(QString::fromUtf8("horizontalLayout_27"));
        label_5 = new QLabel(streamline_cpuClass);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_27->addWidget(label_5);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        checkBox_UseLineColor = new QCheckBox(streamline_cpuClass);
        checkBox_UseLineColor->setObjectName(QString::fromUtf8("checkBox_UseLineColor"));
        checkBox_UseLineColor->setChecked(false);

        verticalLayout_2->addWidget(checkBox_UseLineColor);

        checkBox_ShowArrow = new QCheckBox(streamline_cpuClass);
        checkBox_ShowArrow->setObjectName(QString::fromUtf8("checkBox_ShowArrow"));

        verticalLayout_2->addWidget(checkBox_ShowArrow);


        horizontalLayout_27->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_6 = new QLabel(streamline_cpuClass);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        horizontalLayout_5->addWidget(label_6);

        horizontalSlider_ArrowR = new QSlider(streamline_cpuClass);
        horizontalSlider_ArrowR->setObjectName(QString::fromUtf8("horizontalSlider_ArrowR"));
        horizontalSlider_ArrowR->setMaximum(255);
        horizontalSlider_ArrowR->setSliderPosition(255);
        horizontalSlider_ArrowR->setOrientation(Qt::Horizontal);

        horizontalLayout_5->addWidget(horizontalSlider_ArrowR);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_7 = new QLabel(streamline_cpuClass);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_6->addWidget(label_7);

        horizontalSlider_ArrowG = new QSlider(streamline_cpuClass);
        horizontalSlider_ArrowG->setObjectName(QString::fromUtf8("horizontalSlider_ArrowG"));
        horizontalSlider_ArrowG->setMaximum(255);
        horizontalSlider_ArrowG->setSliderPosition(255);
        horizontalSlider_ArrowG->setOrientation(Qt::Horizontal);

        horizontalLayout_6->addWidget(horizontalSlider_ArrowG);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        label_8 = new QLabel(streamline_cpuClass);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_7->addWidget(label_8);

        horizontalSlider_ArrowB = new QSlider(streamline_cpuClass);
        horizontalSlider_ArrowB->setObjectName(QString::fromUtf8("horizontalSlider_ArrowB"));
        horizontalSlider_ArrowB->setMaximum(255);
        horizontalSlider_ArrowB->setSliderPosition(255);
        horizontalSlider_ArrowB->setOrientation(Qt::Horizontal);

        horizontalLayout_7->addWidget(horizontalSlider_ArrowB);


        verticalLayout->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_9 = new QLabel(streamline_cpuClass);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        horizontalLayout_8->addWidget(label_9);

        horizontalSlider_ArrowA = new QSlider(streamline_cpuClass);
        horizontalSlider_ArrowA->setObjectName(QString::fromUtf8("horizontalSlider_ArrowA"));
        horizontalSlider_ArrowA->setMaximum(255);
        horizontalSlider_ArrowA->setSliderPosition(255);
        horizontalSlider_ArrowA->setOrientation(Qt::Horizontal);

        horizontalLayout_8->addWidget(horizontalSlider_ArrowA);


        verticalLayout->addLayout(horizontalLayout_8);


        horizontalLayout_27->addLayout(verticalLayout);


        gridLayout_5->addLayout(horizontalLayout_27, 2, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(streamline_cpuClass);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        label_16 = new QLabel(streamline_cpuClass);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        horizontalLayout_16->addWidget(label_16);

        horizontalSlider_SelectionR = new QSlider(streamline_cpuClass);
        horizontalSlider_SelectionR->setObjectName(QString::fromUtf8("horizontalSlider_SelectionR"));
        horizontalSlider_SelectionR->setMaximum(255);
        horizontalSlider_SelectionR->setSliderPosition(255);
        horizontalSlider_SelectionR->setOrientation(Qt::Horizontal);

        horizontalLayout_16->addWidget(horizontalSlider_SelectionR);


        verticalLayout_3->addLayout(horizontalLayout_16);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        label_17 = new QLabel(streamline_cpuClass);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        horizontalLayout_17->addWidget(label_17);

        horizontalSlider_SelectionG = new QSlider(streamline_cpuClass);
        horizontalSlider_SelectionG->setObjectName(QString::fromUtf8("horizontalSlider_SelectionG"));
        horizontalSlider_SelectionG->setMaximum(255);
        horizontalSlider_SelectionG->setSliderPosition(0);
        horizontalSlider_SelectionG->setOrientation(Qt::Horizontal);

        horizontalLayout_17->addWidget(horizontalSlider_SelectionG);


        verticalLayout_3->addLayout(horizontalLayout_17);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        label_18 = new QLabel(streamline_cpuClass);
        label_18->setObjectName(QString::fromUtf8("label_18"));

        horizontalLayout_18->addWidget(label_18);

        horizontalSlider_SelectionB = new QSlider(streamline_cpuClass);
        horizontalSlider_SelectionB->setObjectName(QString::fromUtf8("horizontalSlider_SelectionB"));
        horizontalSlider_SelectionB->setMaximum(255);
        horizontalSlider_SelectionB->setSliderPosition(0);
        horizontalSlider_SelectionB->setOrientation(Qt::Horizontal);

        horizontalLayout_18->addWidget(horizontalSlider_SelectionB);


        verticalLayout_3->addLayout(horizontalLayout_18);

        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName(QString::fromUtf8("horizontalLayout_19"));
        label_19 = new QLabel(streamline_cpuClass);
        label_19->setObjectName(QString::fromUtf8("label_19"));

        horizontalLayout_19->addWidget(label_19);

        horizontalSlider_SelectionA = new QSlider(streamline_cpuClass);
        horizontalSlider_SelectionA->setObjectName(QString::fromUtf8("horizontalSlider_SelectionA"));
        horizontalSlider_SelectionA->setMaximum(255);
        horizontalSlider_SelectionA->setSliderPosition(255);
        horizontalSlider_SelectionA->setOrientation(Qt::Horizontal);

        horizontalLayout_19->addWidget(horizontalSlider_SelectionA);


        verticalLayout_3->addLayout(horizontalLayout_19);


        horizontalLayout_3->addLayout(verticalLayout_3);


        gridLayout_5->addLayout(horizontalLayout_3, 2, 1, 1, 1);

        horizontalLayout_28 = new QHBoxLayout();
        horizontalLayout_28->setObjectName(QString::fromUtf8("horizontalLayout_28"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(streamline_cpuClass);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        horizontalSlider_LineWidth = new QSlider(streamline_cpuClass);
        horizontalSlider_LineWidth->setObjectName(QString::fromUtf8("horizontalSlider_LineWidth"));
        horizontalSlider_LineWidth->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(horizontalSlider_LineWidth);


        verticalLayout_6->addLayout(horizontalLayout);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        label_15 = new QLabel(streamline_cpuClass);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        horizontalLayout_13->addWidget(label_15);

        horizontalSlider_FadeOpacity = new QSlider(streamline_cpuClass);
        horizontalSlider_FadeOpacity->setObjectName(QString::fromUtf8("horizontalSlider_FadeOpacity"));
        horizontalSlider_FadeOpacity->setOrientation(Qt::Horizontal);

        horizontalLayout_13->addWidget(horizontalSlider_FadeOpacity);


        verticalLayout_6->addLayout(horizontalLayout_13);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        label_13 = new QLabel(streamline_cpuClass);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        horizontalLayout_12->addWidget(label_13);

        horizontalSlider_SpeedScaleFactor = new QSlider(streamline_cpuClass);
        horizontalSlider_SpeedScaleFactor->setObjectName(QString::fromUtf8("horizontalSlider_SpeedScaleFactor"));
        horizontalSlider_SpeedScaleFactor->setOrientation(Qt::Horizontal);

        horizontalLayout_12->addWidget(horizontalSlider_SpeedScaleFactor);


        verticalLayout_6->addLayout(horizontalLayout_12);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        label_12 = new QLabel(streamline_cpuClass);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_14->addWidget(label_12);

        horizontalSlider_H = new QSlider(streamline_cpuClass);
        horizontalSlider_H->setObjectName(QString::fromUtf8("horizontalSlider_H"));
        horizontalSlider_H->setOrientation(Qt::Horizontal);

        horizontalLayout_14->addWidget(horizontalSlider_H);


        verticalLayout_6->addLayout(horizontalLayout_14);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        label_14 = new QLabel(streamline_cpuClass);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        horizontalLayout_15->addWidget(label_14);

        horizontalSlider_PointDensity = new QSlider(streamline_cpuClass);
        horizontalSlider_PointDensity->setObjectName(QString::fromUtf8("horizontalSlider_PointDensity"));
        horizontalSlider_PointDensity->setOrientation(Qt::Horizontal);

        horizontalLayout_15->addWidget(horizontalSlider_PointDensity);


        verticalLayout_6->addLayout(horizontalLayout_15);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(streamline_cpuClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        comboBox_LineStyle = new QComboBox(streamline_cpuClass);
        comboBox_LineStyle->addItem(QString());
        comboBox_LineStyle->addItem(QString());
        comboBox_LineStyle->setObjectName(QString::fromUtf8("comboBox_LineStyle"));

        horizontalLayout_2->addWidget(comboBox_LineStyle);


        verticalLayout_6->addLayout(horizontalLayout_2);


        horizontalLayout_28->addLayout(verticalLayout_6);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_25 = new QLabel(streamline_cpuClass);
        label_25->setObjectName(QString::fromUtf8("label_25"));

        horizontalLayout_4->addWidget(label_25);

        comboBox_DifferentialMethod = new QComboBox(streamline_cpuClass);
        comboBox_DifferentialMethod->addItem(QString());
        comboBox_DifferentialMethod->addItem(QString());
        comboBox_DifferentialMethod->setObjectName(QString::fromUtf8("comboBox_DifferentialMethod"));

        horizontalLayout_4->addWidget(comboBox_DifferentialMethod);


        verticalLayout_7->addLayout(horizontalLayout_4);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        label_11 = new QLabel(streamline_cpuClass);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        horizontalLayout_11->addWidget(label_11);

        comboBox_Smooth = new QComboBox(streamline_cpuClass);
        comboBox_Smooth->addItem(QString());
        comboBox_Smooth->addItem(QString());
        comboBox_Smooth->addItem(QString());
        comboBox_Smooth->setObjectName(QString::fromUtf8("comboBox_Smooth"));

        horizontalLayout_11->addWidget(comboBox_Smooth);


        verticalLayout_7->addLayout(horizontalLayout_11);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        label_10 = new QLabel(streamline_cpuClass);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        horizontalLayout_10->addWidget(label_10);

        comboBox_RandomSeedSelection = new QComboBox(streamline_cpuClass);
        comboBox_RandomSeedSelection->addItem(QString());
        comboBox_RandomSeedSelection->addItem(QString());
        comboBox_RandomSeedSelection->addItem(QString());
        comboBox_RandomSeedSelection->setObjectName(QString::fromUtf8("comboBox_RandomSeedSelection"));

        horizontalLayout_10->addWidget(comboBox_RandomSeedSelection);


        verticalLayout_7->addLayout(horizontalLayout_10);


        horizontalLayout_28->addLayout(verticalLayout_7);


        gridLayout_5->addLayout(horizontalLayout_28, 1, 0, 1, 2);


        retranslateUi(streamline_cpuClass);

        QMetaObject::connectSlotsByName(streamline_cpuClass);
    } // setupUi

    void retranslateUi(QWidget *streamline_cpuClass)
    {
        streamline_cpuClass->setWindowTitle(QApplication::translate("streamline_cpuClass", "StreamLine", nullptr));
        label_4->setText(QApplication::translate("streamline_cpuClass", "Line Color", nullptr));
        label_24->setText(QApplication::translate("streamline_cpuClass", "Color Template", nullptr));
        comboBox_ColorTemplate->setItemText(0, QApplication::translate("streamline_cpuClass", "Rainbow", nullptr));

        label_30->setText(QApplication::translate("streamline_cpuClass", "HSV(Multi Color) Adjustment", nullptr));
        label_26->setText(QApplication::translate("streamline_cpuClass", "Hue Range", nullptr));
        label_27->setText(QApplication::translate("streamline_cpuClass", "Hue Offset", nullptr));
        label_28->setText(QApplication::translate("streamline_cpuClass", "Saturation", nullptr));
        label_29->setText(QApplication::translate("streamline_cpuClass", "Value", nullptr));
        label_31->setText(QApplication::translate("streamline_cpuClass", "RGBA(Single Color) Adjustment", nullptr));
        checkBox_UseSingleColor->setText(QApplication::translate("streamline_cpuClass", "Use Single Color", nullptr));
        label_20->setText(QApplication::translate("streamline_cpuClass", "R", nullptr));
        label_21->setText(QApplication::translate("streamline_cpuClass", "G", nullptr));
        label_22->setText(QApplication::translate("streamline_cpuClass", "B", nullptr));
        label_23->setText(QApplication::translate("streamline_cpuClass", "A", nullptr));
        label_5->setText(QApplication::translate("streamline_cpuClass", "Arrow", nullptr));
        checkBox_UseLineColor->setText(QApplication::translate("streamline_cpuClass", "Use Line Color", nullptr));
        checkBox_ShowArrow->setText(QApplication::translate("streamline_cpuClass", "Show Arrow", nullptr));
        label_6->setText(QApplication::translate("streamline_cpuClass", "R", nullptr));
        label_7->setText(QApplication::translate("streamline_cpuClass", "G", nullptr));
        label_8->setText(QApplication::translate("streamline_cpuClass", "B", nullptr));
        label_9->setText(QApplication::translate("streamline_cpuClass", "A", nullptr));
        label_3->setText(QApplication::translate("streamline_cpuClass", "Selection", nullptr));
        label_16->setText(QApplication::translate("streamline_cpuClass", "R", nullptr));
        label_17->setText(QApplication::translate("streamline_cpuClass", "G", nullptr));
        label_18->setText(QApplication::translate("streamline_cpuClass", "B", nullptr));
        label_19->setText(QApplication::translate("streamline_cpuClass", "A", nullptr));
        label->setText(QApplication::translate("streamline_cpuClass", "Line Width", nullptr));
        label_15->setText(QApplication::translate("streamline_cpuClass", "Fade Opacity", nullptr));
        label_13->setText(QApplication::translate("streamline_cpuClass", "Speed Scale Factor", nullptr));
        label_12->setText(QApplication::translate("streamline_cpuClass", "H", nullptr));
        label_14->setText(QApplication::translate("streamline_cpuClass", "Point Density", nullptr));
        label_2->setText(QApplication::translate("streamline_cpuClass", "Line Style", nullptr));
        comboBox_LineStyle->setItemText(0, QApplication::translate("streamline_cpuClass", "Solid Line", nullptr));
        comboBox_LineStyle->setItemText(1, QApplication::translate("streamline_cpuClass", "Dotted Line", nullptr));

        label_25->setText(QApplication::translate("streamline_cpuClass", "Differential Method", nullptr));
        comboBox_DifferentialMethod->setItemText(0, QApplication::translate("streamline_cpuClass", "Forward Euler", nullptr));
        comboBox_DifferentialMethod->setItemText(1, QApplication::translate("streamline_cpuClass", "Runge-Kutta (Second Order)", nullptr));

        label_11->setText(QApplication::translate("streamline_cpuClass", "Smooth Algorithm", nullptr));
        comboBox_Smooth->setItemText(0, QApplication::translate("streamline_cpuClass", "Interpolated Straight Line", nullptr));
        comboBox_Smooth->setItemText(1, QApplication::translate("streamline_cpuClass", "Circular Arc Spline", nullptr));
        comboBox_Smooth->setItemText(2, QApplication::translate("streamline_cpuClass", "Quadratic Bezier", nullptr));

        label_10->setText(QApplication::translate("streamline_cpuClass", "Random Seed Selection", nullptr));
        comboBox_RandomSeedSelection->setItemText(0, QApplication::translate("streamline_cpuClass", "Pseudo Random", nullptr));
        comboBox_RandomSeedSelection->setItemText(1, QApplication::translate("streamline_cpuClass", "Density Based", nullptr));
        comboBox_RandomSeedSelection->setItemText(2, QApplication::translate("streamline_cpuClass", "Information Entropy Based", nullptr));

    } // retranslateUi

};

namespace Ui {
    class streamline_cpuClass: public Ui_streamline_cpuClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STREAMLINE_CPU_H
