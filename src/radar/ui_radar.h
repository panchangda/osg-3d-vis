/********************************************************************************
** Form generated from reading UI file 'radar.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RADAR_H
#define UI_RADAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RadarUi
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_11;
    QLabel *label_7;
    QLabel *label_5;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_10;
    QDoubleSpinBox *lineWidth;
    QComboBox *DisplayList;
    QDoubleSpinBox *ColorB;
    QLabel *label_2;
    QLabel *label_9;
    QDoubleSpinBox *ColorR;
    QLabel *label_13;
    QDoubleSpinBox *ColorA;
    QLabel *label_8;
    QDoubleSpinBox *ColorG;
    QLabel *label_12;
    QDoubleSpinBox *EMIR;
    QDoubleSpinBox *EMIG;
    QDoubleSpinBox *EMIB;
    QDoubleSpinBox *EMIPHI;
    QDoubleSpinBox *EMITHETA;

    void setupUi(QWidget *RadarUi)
    {
        if (RadarUi->objectName().isEmpty())
            RadarUi->setObjectName(QString::fromUtf8("RadarUi"));
        RadarUi->resize(638, 380);
        gridLayoutWidget = new QWidget(RadarUi);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 30, 571, 311));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(gridLayoutWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout->addWidget(label_11, 3, 4, 1, 1);

        label_7 = new QLabel(gridLayoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 1, 0, 2, 1);

        label_5 = new QLabel(gridLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setEnabled(false);

        gridLayout->addWidget(label_5, 1, 6, 1, 1);

        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setEnabled(false);

        gridLayout->addWidget(label_4, 1, 5, 1, 1);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setEnabled(false);

        gridLayout->addWidget(label_6, 1, 7, 1, 1);

        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setEnabled(false);

        gridLayout->addWidget(label, 1, 1, 1, 1);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setEnabled(false);

        gridLayout->addWidget(label_3, 1, 4, 1, 1);

        label_10 = new QLabel(gridLayoutWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 3, 3, 1, 1);

        lineWidth = new QDoubleSpinBox(gridLayoutWidget);
        lineWidth->setObjectName(QString::fromUtf8("lineWidth"));
        lineWidth->setMinimum(1.000000000000000);
        lineWidth->setMaximum(5.000000000000000);
        lineWidth->setValue(2.000000000000000);

        gridLayout->addWidget(lineWidth, 2, 7, 1, 1);

        DisplayList = new QComboBox(gridLayoutWidget);
        DisplayList->addItem(QString());
        DisplayList->addItem(QString());
        DisplayList->setObjectName(QString::fromUtf8("DisplayList"));

        gridLayout->addWidget(DisplayList, 2, 1, 1, 1);

        ColorB = new QDoubleSpinBox(gridLayoutWidget);
        ColorB->setObjectName(QString::fromUtf8("ColorB"));
        ColorB->setMaximum(1.000000000000000);
        ColorB->setSingleStep(0.010000000000000);
        ColorB->setValue(0.600000000000000);

        gridLayout->addWidget(ColorB, 2, 5, 1, 1);

        label_2 = new QLabel(gridLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setEnabled(false);

        gridLayout->addWidget(label_2, 1, 3, 1, 1);

        label_9 = new QLabel(gridLayoutWidget);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 3, 1, 1, 1);

        ColorR = new QDoubleSpinBox(gridLayoutWidget);
        ColorR->setObjectName(QString::fromUtf8("ColorR"));
        ColorR->setMaximum(1.000000000000000);
        ColorR->setSingleStep(0.100000000000000);
        ColorR->setValue(1.000000000000000);

        gridLayout->addWidget(ColorR, 2, 3, 1, 1);

        label_13 = new QLabel(gridLayoutWidget);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        gridLayout->addWidget(label_13, 3, 6, 1, 1);

        ColorA = new QDoubleSpinBox(gridLayoutWidget);
        ColorA->setObjectName(QString::fromUtf8("ColorA"));
        ColorA->setMaximum(1.000000000000000);
        ColorA->setSingleStep(0.010000000000000);
        ColorA->setValue(0.400000000000000);

        gridLayout->addWidget(ColorA, 2, 6, 1, 1);

        label_8 = new QLabel(gridLayoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout->addWidget(label_8, 3, 0, 1, 1);

        ColorG = new QDoubleSpinBox(gridLayoutWidget);
        ColorG->setObjectName(QString::fromUtf8("ColorG"));
        ColorG->setMaximum(1.000000000000000);
        ColorG->setSingleStep(0.010000000000000);
        ColorG->setValue(0.600000000000000);

        gridLayout->addWidget(ColorG, 2, 4, 1, 1);

        label_12 = new QLabel(gridLayoutWidget);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        gridLayout->addWidget(label_12, 3, 5, 1, 1);

        EMIR = new QDoubleSpinBox(gridLayoutWidget);
        EMIR->setObjectName(QString::fromUtf8("EMIR"));
        EMIR->setMaximum(1.000000000000000);
        EMIR->setSingleStep(0.010000000000000);
        EMIR->setValue(0.500000000000000);

        gridLayout->addWidget(EMIR, 4, 1, 1, 1);

        EMIG = new QDoubleSpinBox(gridLayoutWidget);
        EMIG->setObjectName(QString::fromUtf8("EMIG"));
        EMIG->setMaximum(1.000000000000000);
        EMIG->setSingleStep(0.010000000000000);
        EMIG->setValue(1.000000000000000);

        gridLayout->addWidget(EMIG, 4, 3, 1, 1);

        EMIB = new QDoubleSpinBox(gridLayoutWidget);
        EMIB->setObjectName(QString::fromUtf8("EMIB"));
        EMIB->setMaximum(1.000000000000000);
        EMIB->setSingleStep(0.010000000000000);
        EMIB->setValue(0.440000000000000);

        gridLayout->addWidget(EMIB, 4, 4, 1, 1);

        EMIPHI = new QDoubleSpinBox(gridLayoutWidget);
        EMIPHI->setObjectName(QString::fromUtf8("EMIPHI"));
        EMIPHI->setMaximum(0.500000000000000);
        EMIPHI->setSingleStep(0.001000000000000);
        EMIPHI->setValue(0.100000000000000);

        gridLayout->addWidget(EMIPHI, 4, 5, 1, 1);

        EMITHETA = new QDoubleSpinBox(gridLayoutWidget);
        EMITHETA->setObjectName(QString::fromUtf8("EMITHETA"));
        EMITHETA->setMaximum(0.500000000000000);
        EMITHETA->setSingleStep(0.001000000000000);
        EMITHETA->setValue(0.050000000000000);

        gridLayout->addWidget(EMITHETA, 4, 6, 1, 1);


        retranslateUi(RadarUi);

        QMetaObject::connectSlotsByName(RadarUi);
    } // setupUi

    void retranslateUi(QWidget *RadarUi)
    {
        RadarUi->setWindowTitle(QApplication::translate("RadarUi", "RadarUi", nullptr));
        label_11->setText(QApplication::translate("RadarUi", "Color\357\274\232B", nullptr));
        label_7->setText(QApplication::translate("RadarUi", "RadarPart", nullptr));
        label_5->setText(QApplication::translate("RadarUi", "Color:A", nullptr));
        label_4->setText(QApplication::translate("RadarUi", "Color:B", nullptr));
        label_6->setText(QApplication::translate("RadarUi", "Line_Width", nullptr));
        label->setText(QApplication::translate("RadarUi", "DisplayMode", nullptr));
        label_3->setText(QApplication::translate("RadarUi", "Color:G", nullptr));
        label_10->setText(QApplication::translate("RadarUi", "Color\357\274\232G", nullptr));
        DisplayList->setItemText(0, QApplication::translate("RadarUi", "surface", nullptr));
        DisplayList->setItemText(1, QApplication::translate("RadarUi", "line", nullptr));

        label_2->setText(QApplication::translate("RadarUi", "Color:R", nullptr));
        label_9->setText(QApplication::translate("RadarUi", "Color\357\274\232R", nullptr));
        label_13->setText(QApplication::translate("RadarUi", "MoveSpeed:theta", nullptr));
        label_8->setText(QApplication::translate("RadarUi", "EmiPart", nullptr));
        label_12->setText(QApplication::translate("RadarUi", "MoveSpeed:phi", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RadarUi: public Ui_RadarUi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RADAR_H
