#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_radar_generators_manager.h"

#include "RadarGenerator.h"

class RadarGeneratorsManager : public QWidget
{
    Q_OBJECT

public:
    explicit RadarGeneratorsManager(QWidget *parent = nullptr) : QWidget(parent), ui(new Ui::RadarGeneratorManager)
    {
        ui->setupUi(this);
        connect(ui->radarListWidget, &QListWidget::currentRowChanged, this, &RadarGeneratorsManager::onRadarListWidgetCurrentRowChanged);
        connect(ui->updateButton, &QPushButton::clicked, this, &RadarGeneratorsManager::onUpdateButtonClick);
    }
    ~RadarGeneratorsManager() { delete ui; }
    void setRadarList(std::vector<osg::ref_ptr<osg::Group>> &radarList)
    {
        this->radarList = radarList;
        updateRadarListWidget();
    }
    void updateRadarListWidget()
    {
        for (const auto &radar : radarList)
        {
            ui->radarListWidget->addItem(QString(radar->getName().c_str()));
        }
    }

private:
    Ui::RadarGeneratorManager *ui;
    std::vector<osg::ref_ptr<osg::Group>> radarList;

    void onRadarListWidgetCurrentRowChanged(int currentRow)
    {
        if (0 <= currentRow && currentRow < radarList.size())
        {
            auto currentRadar = radarList[currentRow];
            ui->radarNameLable->setText(QString(currentRadar->getName().c_str()));
            updateColorPanels(currentRadar.get());
        }
    }
    void updateColorPanels(osg::Group *radar)
    {
        auto coverageGeode = dynamic_cast<osg::Geode *>(dynamic_cast<osg::MatrixTransform *>(radar->getChild(0))->getChild(0));

        if (coverageGeode)
        {
            std::vector<osg::Vec4> colors(3, osg::Vec4(0, 0, 0, 0));
            auto coverageGeodeStateSet = coverageGeode->getStateSet();
            // PolygonMode
            {
                osg::PolygonMode *polygonMode = dynamic_cast<osg::PolygonMode *>(
                    coverageGeodeStateSet->getAttribute(osg::StateAttribute::POLYGONMODE));
                if (polygonMode)
                {
                    osg::PolygonMode::Mode mode = polygonMode->getMode(osg::PolygonMode::FRONT_AND_BACK);
                    if (mode == osg::PolygonMode::LINE)
                    {
                        ui->polygonModeComboBox->setCurrentIndex(0);
                    }
                    else if (mode == osg::PolygonMode::FILL)
                    {
                        ui->polygonModeComboBox->setCurrentIndex(1);
                    }
                }
            }
            // LineWidth
            {
                osg::LineWidth *lineWidth = dynamic_cast<osg::LineWidth *>(
                    coverageGeodeStateSet->getAttribute(osg::StateAttribute::LINEWIDTH));
                if (lineWidth)
                {
                    float width = lineWidth->getWidth();
                    ui->lineWidthSpinBox->setValue(width);
                }
            }
            for (int i = 0; i < coverageGeode->getNumDrawables(); ++i)
            {
                auto stateSet = coverageGeode->getDrawable(i)->getStateSet();
                auto mainColorUniform = stateSet->getUniform("uMainColor");
                mainColorUniform->get(colors[i]);
            }

#define SETSPINBOXVALUE(idx)                        \
    ui->rSpinBox##idx->setValue(colors[(idx)].r()); \
    ui->gSpinBox##idx->setValue(colors[(idx)].g()); \
    ui->bSpinBox##idx->setValue(colors[(idx)].b()); \
    ui->aSpinBox##idx->setValue(colors[(idx)].a());
            SETSPINBOXVALUE(0)
            SETSPINBOXVALUE(1)
            SETSPINBOXVALUE(2)
#undef SETSPINBOXVALUE
        }
    }

    void onUpdateButtonClick()
    {
        int currentRow = ui->radarListWidget->currentRow();

        if (0 <= currentRow && currentRow < radarList.size())
        {
            auto currentRadar = radarList[currentRow];
            auto coverageGeode = dynamic_cast<osg::Geode *>(dynamic_cast<osg::MatrixTransform *>(currentRadar->getChild(0))->getChild(0));
            auto linesGeode = dynamic_cast<osg::Geode *>(dynamic_cast<osg::MatrixTransform *>(currentRadar->getChild(0))->getChild(1));
            if (coverageGeode&&linesGeode)
            {
                auto coverageGeodeStateSet = coverageGeode->getStateSet();
                // PolygonMode
                {
                    osg::PolygonMode *polygonMode = dynamic_cast<osg::PolygonMode *>(
                        coverageGeodeStateSet->getAttribute(osg::StateAttribute::POLYGONMODE));
                    if (polygonMode)
                    {
                        auto polyMode = new osg::PolygonMode;
                        if (ui->polygonModeComboBox->currentIndex() == 0)
                        { // LINE

                            polyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
                            coverageGeode->setNodeMask(0x00);
                            linesGeode->setNodeMask(0xff);
                        }
                        else if (ui->polygonModeComboBox->currentIndex() == 1)
                        { // FILL
                            polyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
                            coverageGeode->setNodeMask(0xff);
                            linesGeode->setNodeMask(0x00);
                        }
                        coverageGeodeStateSet->setAttributeAndModes(polyMode, osg::StateAttribute::ON);
                    }
                }
                // LineWidth
                {
                    osg::LineWidth *lineWidth = dynamic_cast<osg::LineWidth *>(
                        coverageGeodeStateSet->getAttribute(osg::StateAttribute::LINEWIDTH));
                    if (lineWidth)
                    {
                        float width = ui->lineWidthSpinBox->value();
                        lineWidth->setWidth(width);
                    }
                }

                std::vector<osg::Vec4> colors(3, osg::Vec4(0, 0, 0, 0));
#define GETSPINBOXVALUE(idx)                        \
    colors[(idx)].r() = ui->rSpinBox##idx->value(); \
    colors[(idx)].g() = ui->gSpinBox##idx->value(); \
    colors[(idx)].b() = ui->bSpinBox##idx->value(); \
    colors[(idx)].a() = ui->aSpinBox##idx->value();

                GETSPINBOXVALUE(0)
                GETSPINBOXVALUE(1)
                GETSPINBOXVALUE(2)
#undef GETSPINBOXVALUE

                for (int i = 0; i < coverageGeode->getNumDrawables(); ++i)
                {
                    auto stateSet = coverageGeode->getDrawable(i)->getStateSet();
                    auto mainColorUniform = stateSet->getUniform("uMainColor");
                    mainColorUniform->set(colors[i]);
                }
            }


//            static double minLat = 48.1, maxLat = 51.5, minLon = 127.2, maxLon = 130.4;
//            osg_3d_vis::llhRange range = { minLat, maxLat, minLon, maxLon, 0, 567732};
//            minLon += 0.50;
//            maxLon += 0.5;
//            Radar::RadarGenerator::setEMI(currentRadar, range);


        }
    }
};
