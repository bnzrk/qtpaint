#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <set>
#include "colorselector.h"
#include "hsvcolorselector.h"

namespace Ui
{
class ColorPanel;
}

class ColorPanel : public QWidget
{
    Q_OBJECT
public:
    enum class Configuration
    {
        None,
        HSV,
        NormalMap
    };
    const std::set<Configuration> configurations = {Configuration::HSV, Configuration::NormalMap};
public:
    explicit ColorPanel(QWidget *parent = nullptr);
public slots:
    void setConfiguration(Configuration config);
private:
    void initConfig(Configuration config);
    void showConfig(Configuration config);
    void hideConfig(Configuration config);
private:
    Ui::ColorPanel* ui;

    Configuration m_config;

    ColorSelector* m_hsvSelector;
};

#endif // COLORPANEL_H
