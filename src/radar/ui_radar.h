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
    QLabel *label;
    QLabel *label_3;
    QDoubleSpinBox *ColorR;
    QLabel *label_6;
    QDoubleSpinBox *ColorB;
    QLabel *HasMSI;
    QLabel *label_2;
    QDoubleSpinBox *ColorG;
    QLabel *label_4;
    QDoubleSpinBox *lineWidth;
    QComboBox *DisplayList;
    QDoubleSpinBox *ColorA;
    QLabel *label_5;
    QComboBox *comboBox;

    void setupUi(QWidget *RadarUi)
    {
        if (RadarUi->objectName().isEmpty())
            RadarUi->setObjectName(QString::fromUtf8("RadarUi"));
        RadarUi->resize(577, 318);
        gridLayoutWidget = new QWidget(RadarUi);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(0, 10, 571, 311));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setEnabled(false);

        gridLayout->addWidget(label, 1, 0, 1, 1);

        label_3 = new QLabel(gridLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setEnabled(false);

        gridLayout->addWidget(label_3, 1, 3, 1, 1);

        ColorR = new QDoubleSpinBox(gridLayoutWidget);
        ColorR->setObjectName(QString::fromUtf8("ColorR"));
        ColorR->setMaximum(1.000000000000000);
        ColorR->setSingleStep(0.100000000000000);
        ColorR->setValue(1.000000000000000);

        gridLayout->addWidget(ColorR, 2, 2, 1, 1);

        label_6 = new QLabel(gridLayoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setEnabled(false);

        gridLayout->addWidget(label_6, 1, 6, 1, 1);

        ColorB = new QDoubleSpinBox(gridLayoutWidget);
        ColorB->setObjectName(QString::fromUtf8("ColorB"));
        ColorB->setMaximum(1.000000000000000);
        ColorB->setSingleStep(0.010000000000000);
        ColorB->setValue(0.600000000000000);

        gridLayout->addWidget(ColorB, 2, 4, 1, 1);

        HasMSI = new QLabel(gridLayoutWidget);
        HasMSI->setObjectName(QString::fromUtf8("HasMSI"));

        gridLayout->addWidget(HasMSI, 3, 0, 1, 1);

        label_2 = new QLabel(gridLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setEnabled(false);

        gridLayout->addWidget(label_2, 1, 2, 1, 1);

        ColorG = new QDoubleSpinBox(gridLayoutWidget);
        ColorG->setObjectName(QString::fromUtf8("ColorG"));
        ColorG->setMaximum(1.000000000000000);
        ColorG->setSingleStep(0.010000000000000);
        ColorG->setValue(0.600000000000000);

        gridLayout->addWidget(ColorG, 2, 3, 1, 1);

        label_4 = new QLabel(gridLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setEnabled(false);

        gridLayout->addWidget(label_4, 1, 4, 1, 1);

        lineWidth = new QDoubleSpinBox(gridLayoutWidget);
        lineWidth->setObjectName(QString::fromUtf8("lineWidth"));
        lineWidth->setMinimum(1.000000000000000);
        lineWidth->setMaximum(5.000000000000000);
        lineWidth->setValue(2.000000000000000);

        gridLayout->addWidget(lineWidth, 2, 6, 1, 1);

        DisplayList = new QComboBox(gridLayoutWidget);
        DisplayList->addItem(QString());
        DisplayList->addItem(QString());
        DisplayList->setObjectName(QString::fromUtf8("DisplayList"));

        gridLayout->addWidget(DisplayList, 2, 0, 1, 1);

        ColorA = new QDoubleSpinBox(gridLayoutWidget);
        ColorA->setObjectName(QString::fromUtf8("ColorA"));
        ColorA->setMaximum(1.000000000000000);
        ColorA->setSingleStep(0.010000000000000);
        ColorA->setValue(0.400000000000000);

        gridLayout->addWidget(ColorA, 2, 5, 1, 1);

        label_5 = new QLabel(gridLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setEnabled(false);

        gridLayout->addWidget(label_5, 1, 5, 1, 1);

        comboBox = new QComboBox(gridLayoutWidget);
        comboBox->addItem(QString());
        comboBox->addItem(QString());
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout->addWidget(comboBox, 3, 2, 1, 5);


        retranslateUi(RadarUi);

        QMetaObject::connectSlotsByName(RadarUi);
    } // setupUi

    void retranslateUi(QWidget *RadarUi)
    {
        RadarUi->setWindowTitle(QApplication::translate("RadarUi", "RadarUi", nullptr));
        label->setText(QApplication::translate("RadarUi", "DisplayMode", nullptr));
        label_3->setText(QApplication::translate("RadarUi", "Color:G", nullptr));
        label_6->setText(QApplication::translate("RadarUi", "Line_Width", nullptr));
        HasMSI->setText(QApplication::translate("RadarUi", "HasEMI", nullptr));
        label_2->setText(QApplication::translate("RadarUi", "Color:R", nullptr));
        label_4->setText(QApplication::translate("RadarUi", "Color:B", nullptr));
        DisplayList->setItemText(0, QApplication::translate("RadarUi", "surface", nullptr));
        DisplayList->setItemText(1, QApplication::translate("RadarUi", "line", nullptr));

        label_5->setText(QApplication::translate("RadarUi", "Color:A", nullptr));
        comboBox->setItemText(0, QApplication::translate("RadarUi", "yes", nullptr));
        comboBox->setItemText(1, QApplication::translate("RadarUi", "no", nullptr));

    } // retranslateUi

};

namespace Ui {
    class RadarUi: public Ui_RadarUi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RADAR_H
