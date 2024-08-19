#include "../include/colorpanel.h"
#include "ui_colorpanel.h"

ColorPanel::ColorPanel(QWidget *parent) :
    QWidget{parent},
    ui(new Ui::ColorPanel),
    m_mainSelectorLayout{nullptr},
    m_config{Configuration::None}
{
    ui->setupUi(this);
    setMouseTracking(true);

    m_mainSelectorLayout = new QStackedLayout;
    ui->mainSelectorContainer->setLayout(m_mainSelectorLayout);
}

void ColorPanel::showEvent(QShowEvent *event)
{
    setConfig(Configuration::HSV);
}

void ColorPanel::setConfig(Configuration config)
{
    if (config == m_config)
        return;

    if (config != Configuration::None && initializedConfigs.count(config) == 0)
    {
        initConfig(config);
    }
    m_config = config;
    showConfig(config);
}

void ColorPanel::initConfig(Configuration config)
{
    if (config == Configuration::None)
        return;

    ColorSelector* newSelector = nullptr;
    if (config == Configuration::HSV)
    {
        newSelector = new HSVColorSelector(this);
        newSelector->setObjectName("hsvSelector");

    }
    else if (config == Configuration::NormalMap)
    {
        newSelector = new NormalMapColorSelector(this);
        newSelector->setObjectName("normalMapSelector");
    }
    if (newSelector)
    {
        int stackIndex = m_mainSelectorLayout->addWidget(newSelector);
        mainSelectors.insert(config, stackIndex);
        initializedConfigs.insert(config);
    }
}

void ColorPanel::showConfig(Configuration config)
{
    if (config == Configuration::None)
    {
        hideCurrentConfig();
        return;
    }

    m_mainSelectorLayout->setCurrentIndex(mainSelectors[config]);
}

void ColorPanel::hideCurrentConfig()
{
    ui->mainSelectorContainer->hide();
}
