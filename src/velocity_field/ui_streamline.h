/********************************************************************************
** Form generated from reading UI file 'streamline.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STREAMLINE_H
#define UI_STREAMLINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_StreamLineMainWindow
{
public:
    QGridLayout *gridLayout;
    QSlider *horizontalSlider_LineWidth;
    QComboBox *comboBox_RandomSeedSelection;
    QLabel *label;
    QSlider *horizontalSlider_DropRate;
    QComboBox *comboBox_Smooth;
    QLabel *label_2;
    QSlider *horizontalSlider_DecayRate;
    QLabel *label_6;
    QLabel *label_4;
    QComboBox *comboBox_ColorTemplate;
    QLabel *label_7;
    QLabel *label_3;
    QSlider *horizontalSlider_SpeedFactor;
    QLabel *label_5;

    void setupUi(QWidget *StreamLineMainWindow)
    {
        if (StreamLineMainWindow->objectName().isEmpty())
            StreamLineMainWindow->setObjectName(QString::fromUtf8("StreamLineMainWindow"));
        StreamLineMainWindow->resize(465, 392);
        gridLayout = new QGridLayout(StreamLineMainWindow);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalSlider_LineWidth = new QSlider(StreamLineMainWindow);
        horizontalSlider_LineWidth->setObjectName(QString::fromUtf8("horizontalSlider_LineWidth"));
        horizontalSlider_LineWidth->setSliderPosition(40);
        horizontalSlider_LineWidth->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSlider_LineWidth, 2, 1, 1, 3);

        comboBox_RandomSeedSelection = new QComboBox(StreamLineMainWindow);
        comboBox_RandomSeedSelection->addItem(QString());
        comboBox_RandomSeedSelection->addItem(QString());
        comboBox_RandomSeedSelection->addItem(QString());
        comboBox_RandomSeedSelection->setObjectName(QString::fromUtf8("comboBox_RandomSeedSelection"));

        gridLayout->addWidget(comboBox_RandomSeedSelection, 4, 2, 1, 1);

        label = new QLabel(StreamLineMainWindow);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        horizontalSlider_DropRate = new QSlider(StreamLineMainWindow);
        horizontalSlider_DropRate->setObjectName(QString::fromUtf8("horizontalSlider_DropRate"));
        horizontalSlider_DropRate->setSliderPosition(3);
        horizontalSlider_DropRate->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSlider_DropRate, 1, 1, 1, 3);

        comboBox_Smooth = new QComboBox(StreamLineMainWindow);
        comboBox_Smooth->addItem(QString());
        comboBox_Smooth->addItem(QString());
        comboBox_Smooth->addItem(QString());
        comboBox_Smooth->setObjectName(QString::fromUtf8("comboBox_Smooth"));

        gridLayout->addWidget(comboBox_Smooth, 5, 2, 1, 1);

        label_2 = new QLabel(StreamLineMainWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        horizontalSlider_DecayRate = new QSlider(StreamLineMainWindow);
        horizontalSlider_DecayRate->setObjectName(QString::fromUtf8("horizontalSlider_DecayRate"));
        horizontalSlider_DecayRate->setSliderPosition(2);
        horizontalSlider_DecayRate->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSlider_DecayRate, 3, 1, 1, 2);

        label_6 = new QLabel(StreamLineMainWindow);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 2, 0, 1, 1);

        label_4 = new QLabel(StreamLineMainWindow);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 5, 0, 1, 1);

        comboBox_ColorTemplate = new QComboBox(StreamLineMainWindow);
        QIcon icon;
        icon.addFile(QString::fromUtf8("../../pcd/shaders/\345\275\251\350\211\262.png"), QSize(), QIcon::Normal, QIcon::Off);
        comboBox_ColorTemplate->addItem(icon, QString());
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../pcd/shaders/t1.png"), QSize(), QIcon::Normal, QIcon::Off);
        comboBox_ColorTemplate->addItem(icon1, QString());
        comboBox_ColorTemplate->addItem(QString());
        comboBox_ColorTemplate->addItem(QString());
        comboBox_ColorTemplate->addItem(QString());
        comboBox_ColorTemplate->setObjectName(QString::fromUtf8("comboBox_ColorTemplate"));

        gridLayout->addWidget(comboBox_ColorTemplate, 6, 2, 1, 1);

        label_7 = new QLabel(StreamLineMainWindow);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout->addWidget(label_7, 3, 0, 1, 1);

        label_3 = new QLabel(StreamLineMainWindow);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setWordWrap(true);

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        horizontalSlider_SpeedFactor = new QSlider(StreamLineMainWindow);
        horizontalSlider_SpeedFactor->setObjectName(QString::fromUtf8("horizontalSlider_SpeedFactor"));
        horizontalSlider_SpeedFactor->setMaximum(100);
        horizontalSlider_SpeedFactor->setSliderPosition(4);
        horizontalSlider_SpeedFactor->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(horizontalSlider_SpeedFactor, 0, 1, 1, 3);

        label_5 = new QLabel(StreamLineMainWindow);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 6, 0, 1, 2);

        gridLayout->setColumnStretch(0, 1);

        retranslateUi(StreamLineMainWindow);

        QMetaObject::connectSlotsByName(StreamLineMainWindow);
    } // setupUi

    void retranslateUi(QWidget *StreamLineMainWindow)
    {
        StreamLineMainWindow->setWindowTitle(QApplication::translate("StreamLineMainWindow", "streamline", nullptr));
        comboBox_RandomSeedSelection->setItemText(0, QApplication::translate("StreamLineMainWindow", "Pseudorandom glsl", nullptr));
        comboBox_RandomSeedSelection->setItemText(1, QApplication::translate("StreamLineMainWindow", "Based on Area Density ", nullptr));
        comboBox_RandomSeedSelection->setItemText(2, QApplication::translate("StreamLineMainWindow", "Based on Area  Information Entropy", nullptr));

        label->setText(QApplication::translate("StreamLineMainWindow", "Speed Factor", nullptr));
        comboBox_Smooth->setItemText(0, QApplication::translate("StreamLineMainWindow", "Straight Line Interpolation", nullptr));
        comboBox_Smooth->setItemText(1, QApplication::translate("StreamLineMainWindow", "Quadratic Bezier", nullptr));
        comboBox_Smooth->setItemText(2, QApplication::translate("StreamLineMainWindow", "Circule Arc Spline", nullptr));

        label_2->setText(QApplication::translate("StreamLineMainWindow", "Drop Rate", nullptr));
        label_6->setText(QApplication::translate("StreamLineMainWindow", "Line Width", nullptr));
        label_4->setText(QApplication::translate("StreamLineMainWindow", "Smooth", nullptr));
        comboBox_ColorTemplate->setItemText(0, QApplication::translate("StreamLineMainWindow", "rainbow", nullptr));
        comboBox_ColorTemplate->setItemText(1, QApplication::translate("StreamLineMainWindow", "Template1", nullptr));
        comboBox_ColorTemplate->setItemText(2, QApplication::translate("StreamLineMainWindow", "Template2", nullptr));
        comboBox_ColorTemplate->setItemText(3, QApplication::translate("StreamLineMainWindow", "Template3", nullptr));
        comboBox_ColorTemplate->setItemText(4, QApplication::translate("StreamLineMainWindow", "Template4", nullptr));

        label_7->setText(QApplication::translate("StreamLineMainWindow", "Decay Rate", nullptr));
        label_3->setText(QApplication::translate("StreamLineMainWindow", "Random Seed\n"
"Selection", nullptr));
        label_5->setText(QApplication::translate("StreamLineMainWindow", "Color Template", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StreamLineMainWindow: public Ui_StreamLineMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STREAMLINE_H
