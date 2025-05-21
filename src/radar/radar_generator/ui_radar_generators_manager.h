/********************************************************************************
** Form generated from reading UI file 'radar_generators_manager.ui'
**
** Created by: Qt User Interface Compiler version 5.15.16
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RADAR_GENERATORS_MANAGER_H
#define UI_RADAR_GENERATORS_MANAGER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RadarGeneratorManager
{
public:
    QFormLayout *formLayout;
    QVBoxLayout *radarListlLayout;
    QLabel *radarListLabel;
    QListWidget *radarListWidget;
    QGroupBox *visualParametersGroupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *polygonModeLayout;
    QLabel *polygonModeLabel;
    QComboBox *polygonModeComboBox;
    QHBoxLayout *horizontalLayout;
    QLabel *lineWidthLabel;
    QDoubleSpinBox *lineWidthSpinBox;
    QLabel *radarNameLable;
    QVBoxLayout *colorPanelsLayout;
    QHBoxLayout *colorPanel0;
    QLabel *colorPanelLabel0;
    QDoubleSpinBox *rSpinBox0;
    QDoubleSpinBox *gSpinBox0;
    QDoubleSpinBox *bSpinBox0;
    QDoubleSpinBox *aSpinBox0;
    QHBoxLayout *colorPanel1;
    QLabel *colorPanelLabel1;
    QDoubleSpinBox *rSpinBox1;
    QDoubleSpinBox *gSpinBox1;
    QDoubleSpinBox *bSpinBox1;
    QDoubleSpinBox *aSpinBox1;
    QHBoxLayout *colorPanel2;
    QLabel *colorPanelLabel2;
    QDoubleSpinBox *rSpinBox2;
    QDoubleSpinBox *gSpinBox2;
    QDoubleSpinBox *bSpinBox2;
    QDoubleSpinBox *aSpinBox2;
    QPushButton *updateButton;

    void setupUi(QWidget *RadarGeneratorManager)
    {
        if (RadarGeneratorManager->objectName().isEmpty())
            RadarGeneratorManager->setObjectName(QString::fromUtf8("RadarGeneratorManager"));
        RadarGeneratorManager->resize(764, 230);
        formLayout = new QFormLayout(RadarGeneratorManager);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        radarListlLayout = new QVBoxLayout();
        radarListlLayout->setObjectName(QString::fromUtf8("radarListlLayout"));
        radarListLabel = new QLabel(RadarGeneratorManager);
        radarListLabel->setObjectName(QString::fromUtf8("radarListLabel"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(radarListLabel->sizePolicy().hasHeightForWidth());
        radarListLabel->setSizePolicy(sizePolicy);

        radarListlLayout->addWidget(radarListLabel);

        radarListWidget = new QListWidget(RadarGeneratorManager);
        radarListWidget->setObjectName(QString::fromUtf8("radarListWidget"));
        radarListWidget->setMinimumSize(QSize(100, 150));
        radarListWidget->setMaximumSize(QSize(200, 16777215));

        radarListlLayout->addWidget(radarListWidget);


        formLayout->setLayout(0, QFormLayout::LabelRole, radarListlLayout);

        visualParametersGroupBox = new QGroupBox(RadarGeneratorManager);
        visualParametersGroupBox->setObjectName(QString::fromUtf8("visualParametersGroupBox"));
        visualParametersGroupBox->setMinimumSize(QSize(300, 200));
        gridLayout = new QGridLayout(visualParametersGroupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        polygonModeLayout = new QHBoxLayout();
        polygonModeLayout->setObjectName(QString::fromUtf8("polygonModeLayout"));
        polygonModeLabel = new QLabel(visualParametersGroupBox);
        polygonModeLabel->setObjectName(QString::fromUtf8("polygonModeLabel"));

        polygonModeLayout->addWidget(polygonModeLabel);

        polygonModeComboBox = new QComboBox(visualParametersGroupBox);
        polygonModeComboBox->addItem(QString());
        polygonModeComboBox->addItem(QString());
        polygonModeComboBox->setObjectName(QString::fromUtf8("polygonModeComboBox"));
        sizePolicy.setHeightForWidth(polygonModeComboBox->sizePolicy().hasHeightForWidth());
        polygonModeComboBox->setSizePolicy(sizePolicy);
        polygonModeComboBox->setMinimumSize(QSize(4, 2));

        polygonModeLayout->addWidget(polygonModeComboBox);


        gridLayout->addLayout(polygonModeLayout, 0, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineWidthLabel = new QLabel(visualParametersGroupBox);
        lineWidthLabel->setObjectName(QString::fromUtf8("lineWidthLabel"));

        horizontalLayout->addWidget(lineWidthLabel);

        lineWidthSpinBox = new QDoubleSpinBox(visualParametersGroupBox);
        lineWidthSpinBox->setObjectName(QString::fromUtf8("lineWidthSpinBox"));
        sizePolicy.setHeightForWidth(lineWidthSpinBox->sizePolicy().hasHeightForWidth());
        lineWidthSpinBox->setSizePolicy(sizePolicy);
        lineWidthSpinBox->setMinimumSize(QSize(4, 2));
        lineWidthSpinBox->setDecimals(1);
        lineWidthSpinBox->setMinimum(1.000000000000000);
        lineWidthSpinBox->setMaximum(10.000000000000000);
        lineWidthSpinBox->setSingleStep(0.100000000000000);

        horizontalLayout->addWidget(lineWidthSpinBox);


        gridLayout->addLayout(horizontalLayout, 0, 1, 1, 1);

        radarNameLable = new QLabel(visualParametersGroupBox);
        radarNameLable->setObjectName(QString::fromUtf8("radarNameLable"));

        gridLayout->addWidget(radarNameLable, 1, 0, 1, 1);

        colorPanelsLayout = new QVBoxLayout();
        colorPanelsLayout->setObjectName(QString::fromUtf8("colorPanelsLayout"));
        colorPanel0 = new QHBoxLayout();
        colorPanel0->setObjectName(QString::fromUtf8("colorPanel0"));
        colorPanelLabel0 = new QLabel(visualParametersGroupBox);
        colorPanelLabel0->setObjectName(QString::fromUtf8("colorPanelLabel0"));

        colorPanel0->addWidget(colorPanelLabel0);

        rSpinBox0 = new QDoubleSpinBox(visualParametersGroupBox);
        rSpinBox0->setObjectName(QString::fromUtf8("rSpinBox0"));
        sizePolicy.setHeightForWidth(rSpinBox0->sizePolicy().hasHeightForWidth());
        rSpinBox0->setSizePolicy(sizePolicy);
        rSpinBox0->setMinimumSize(QSize(4, 2));
        rSpinBox0->setMaximum(1.000000000000000);
        rSpinBox0->setSingleStep(0.010000000000000);

        colorPanel0->addWidget(rSpinBox0);

        gSpinBox0 = new QDoubleSpinBox(visualParametersGroupBox);
        gSpinBox0->setObjectName(QString::fromUtf8("gSpinBox0"));
        sizePolicy.setHeightForWidth(gSpinBox0->sizePolicy().hasHeightForWidth());
        gSpinBox0->setSizePolicy(sizePolicy);
        gSpinBox0->setMinimumSize(QSize(4, 2));
        gSpinBox0->setMaximum(1.000000000000000);
        gSpinBox0->setSingleStep(0.010000000000000);

        colorPanel0->addWidget(gSpinBox0);

        bSpinBox0 = new QDoubleSpinBox(visualParametersGroupBox);
        bSpinBox0->setObjectName(QString::fromUtf8("bSpinBox0"));
        sizePolicy.setHeightForWidth(bSpinBox0->sizePolicy().hasHeightForWidth());
        bSpinBox0->setSizePolicy(sizePolicy);
        bSpinBox0->setMinimumSize(QSize(4, 2));
        bSpinBox0->setMaximum(1.000000000000000);
        bSpinBox0->setSingleStep(0.010000000000000);

        colorPanel0->addWidget(bSpinBox0);

        aSpinBox0 = new QDoubleSpinBox(visualParametersGroupBox);
        aSpinBox0->setObjectName(QString::fromUtf8("aSpinBox0"));
        sizePolicy.setHeightForWidth(aSpinBox0->sizePolicy().hasHeightForWidth());
        aSpinBox0->setSizePolicy(sizePolicy);
        aSpinBox0->setMinimumSize(QSize(4, 2));
        aSpinBox0->setMaximum(1.000000000000000);
        aSpinBox0->setSingleStep(0.010000000000000);

        colorPanel0->addWidget(aSpinBox0);


        colorPanelsLayout->addLayout(colorPanel0);

        colorPanel1 = new QHBoxLayout();
        colorPanel1->setObjectName(QString::fromUtf8("colorPanel1"));
        colorPanelLabel1 = new QLabel(visualParametersGroupBox);
        colorPanelLabel1->setObjectName(QString::fromUtf8("colorPanelLabel1"));

        colorPanel1->addWidget(colorPanelLabel1);

        rSpinBox1 = new QDoubleSpinBox(visualParametersGroupBox);
        rSpinBox1->setObjectName(QString::fromUtf8("rSpinBox1"));
        sizePolicy.setHeightForWidth(rSpinBox1->sizePolicy().hasHeightForWidth());
        rSpinBox1->setSizePolicy(sizePolicy);
        rSpinBox1->setMinimumSize(QSize(4, 2));
        rSpinBox1->setMaximum(1.000000000000000);
        rSpinBox1->setSingleStep(0.010000000000000);

        colorPanel1->addWidget(rSpinBox1);

        gSpinBox1 = new QDoubleSpinBox(visualParametersGroupBox);
        gSpinBox1->setObjectName(QString::fromUtf8("gSpinBox1"));
        sizePolicy.setHeightForWidth(gSpinBox1->sizePolicy().hasHeightForWidth());
        gSpinBox1->setSizePolicy(sizePolicy);
        gSpinBox1->setMinimumSize(QSize(4, 2));
        gSpinBox1->setMaximum(1.000000000000000);
        gSpinBox1->setSingleStep(0.010000000000000);

        colorPanel1->addWidget(gSpinBox1);

        bSpinBox1 = new QDoubleSpinBox(visualParametersGroupBox);
        bSpinBox1->setObjectName(QString::fromUtf8("bSpinBox1"));
        sizePolicy.setHeightForWidth(bSpinBox1->sizePolicy().hasHeightForWidth());
        bSpinBox1->setSizePolicy(sizePolicy);
        bSpinBox1->setMinimumSize(QSize(4, 2));
        bSpinBox1->setMaximum(1.000000000000000);
        bSpinBox1->setSingleStep(0.010000000000000);

        colorPanel1->addWidget(bSpinBox1);

        aSpinBox1 = new QDoubleSpinBox(visualParametersGroupBox);
        aSpinBox1->setObjectName(QString::fromUtf8("aSpinBox1"));
        sizePolicy.setHeightForWidth(aSpinBox1->sizePolicy().hasHeightForWidth());
        aSpinBox1->setSizePolicy(sizePolicy);
        aSpinBox1->setMinimumSize(QSize(4, 2));
        aSpinBox1->setMaximum(1.000000000000000);
        aSpinBox1->setSingleStep(0.010000000000000);

        colorPanel1->addWidget(aSpinBox1);


        colorPanelsLayout->addLayout(colorPanel1);

        colorPanel2 = new QHBoxLayout();
        colorPanel2->setObjectName(QString::fromUtf8("colorPanel2"));
        colorPanelLabel2 = new QLabel(visualParametersGroupBox);
        colorPanelLabel2->setObjectName(QString::fromUtf8("colorPanelLabel2"));

        colorPanel2->addWidget(colorPanelLabel2);

        rSpinBox2 = new QDoubleSpinBox(visualParametersGroupBox);
        rSpinBox2->setObjectName(QString::fromUtf8("rSpinBox2"));
        sizePolicy.setHeightForWidth(rSpinBox2->sizePolicy().hasHeightForWidth());
        rSpinBox2->setSizePolicy(sizePolicy);
        rSpinBox2->setMinimumSize(QSize(4, 2));
        rSpinBox2->setMaximum(1.000000000000000);
        rSpinBox2->setSingleStep(0.010000000000000);

        colorPanel2->addWidget(rSpinBox2);

        gSpinBox2 = new QDoubleSpinBox(visualParametersGroupBox);
        gSpinBox2->setObjectName(QString::fromUtf8("gSpinBox2"));
        sizePolicy.setHeightForWidth(gSpinBox2->sizePolicy().hasHeightForWidth());
        gSpinBox2->setSizePolicy(sizePolicy);
        gSpinBox2->setMinimumSize(QSize(4, 2));
        gSpinBox2->setMaximum(1.000000000000000);
        gSpinBox2->setSingleStep(0.010000000000000);

        colorPanel2->addWidget(gSpinBox2);

        bSpinBox2 = new QDoubleSpinBox(visualParametersGroupBox);
        bSpinBox2->setObjectName(QString::fromUtf8("bSpinBox2"));
        sizePolicy.setHeightForWidth(bSpinBox2->sizePolicy().hasHeightForWidth());
        bSpinBox2->setSizePolicy(sizePolicy);
        bSpinBox2->setMinimumSize(QSize(4, 2));
        bSpinBox2->setMaximum(1.000000000000000);
        bSpinBox2->setSingleStep(0.010000000000000);

        colorPanel2->addWidget(bSpinBox2);

        aSpinBox2 = new QDoubleSpinBox(visualParametersGroupBox);
        aSpinBox2->setObjectName(QString::fromUtf8("aSpinBox2"));
        sizePolicy.setHeightForWidth(aSpinBox2->sizePolicy().hasHeightForWidth());
        aSpinBox2->setSizePolicy(sizePolicy);
        aSpinBox2->setMinimumSize(QSize(4, 2));
        aSpinBox2->setMaximum(1.000000000000000);
        aSpinBox2->setSingleStep(0.010000000000000);

        colorPanel2->addWidget(aSpinBox2);


        colorPanelsLayout->addLayout(colorPanel2);


        gridLayout->addLayout(colorPanelsLayout, 2, 0, 1, 2);

        updateButton = new QPushButton(visualParametersGroupBox);
        updateButton->setObjectName(QString::fromUtf8("updateButton"));

        gridLayout->addWidget(updateButton, 3, 1, 1, 1);


        formLayout->setWidget(0, QFormLayout::FieldRole, visualParametersGroupBox);


        retranslateUi(RadarGeneratorManager);

        QMetaObject::connectSlotsByName(RadarGeneratorManager);
    } // setupUi

    void retranslateUi(QWidget *RadarGeneratorManager)
    {
        RadarGeneratorManager->setWindowTitle(QCoreApplication::translate("RadarGeneratorManager", "Radar Generators Manager", nullptr));
        radarListLabel->setText(QCoreApplication::translate("RadarGeneratorManager", "Radar List", nullptr));
        polygonModeLabel->setText(QCoreApplication::translate("RadarGeneratorManager", "PolygonMode", nullptr));
        polygonModeComboBox->setItemText(0, QCoreApplication::translate("RadarGeneratorManager", "Line", nullptr));
        polygonModeComboBox->setItemText(1, QCoreApplication::translate("RadarGeneratorManager", "Fill", nullptr));

        lineWidthLabel->setText(QCoreApplication::translate("RadarGeneratorManager", "lineWidth", nullptr));
        radarNameLable->setText(QString());
        colorPanelLabel0->setText(QCoreApplication::translate("RadarGeneratorManager", "RGBA", nullptr));
        colorPanelLabel1->setText(QCoreApplication::translate("RadarGeneratorManager", "RGBA", nullptr));
        colorPanelLabel2->setText(QCoreApplication::translate("RadarGeneratorManager", "RGBA", nullptr));
        updateButton->setText(QCoreApplication::translate("RadarGeneratorManager", "Update", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RadarGeneratorManager: public Ui_RadarGeneratorManager {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RADAR_GENERATORS_MANAGER_H
