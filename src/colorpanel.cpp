#include "../include/colorpanel.h"
#include "ui_colorpanel.h"

ColorPanel::ColorPanel(QWidget *parent) :
    QWidget{parent},
    ui(new Ui::ColorPanel),
    m_hsvSelector{nullptr},
    m_config{Configuration::None}
{
    ui->setupUi(this);

    setConfiguration(Configuration::HSV);
}

void ColorPanel::setConfiguration(Configuration config)
{
    if (config == Configuration::HSV)
    {
        if (!m_hsvSelector)
            initConfig(config);

        hideConfig(m_config);
        m_config = config;
        showConfig(config);
    }
    else if (config == Configuration::None)
    {
        for (auto c : configurations)
        {
            hideConfig(c);
        }
        m_config = config;
    }
}

void ColorPanel::initConfig(Configuration config)
{
    if (config == Configuration::HSV)
    {
        m_hsvSelector = new HSVColorSelector(this);
        m_hsvSelector->setObjectName("hsvSelector");
    }
}

void ColorPanel::showConfig(Configuration config)
{
    if (config == Configuration::HSV)
    {
        ui->mainSelectorLayout->takeAt(0);
        ui->mainSelectorLayout->addWidget(m_hsvSelector);
        m_hsvSelector->show();
    }
}

void ColorPanel::hideConfig(Configuration config)
{
    if (config == Configuration::HSV)
    {
        m_hsvSelector->disconnect();
        m_hsvSelector->hide();
    }
}
