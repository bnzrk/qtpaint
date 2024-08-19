#ifndef COLORPANEL_H
#define COLORPANEL_H

#include <QStackedLayout>
#include <QMap>
#include <set>
#include "hsvcolorselector.h"
#include "normalmapcolorselector.h"
#include <QMouseEvent>

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

    void showEvent(QShowEvent* event) override;
public slots:
    void setConfig(Configuration config);
private:
    void initConfig(Configuration config);
    void showConfig(Configuration config);
    void hideCurrentConfig();
private:
    Ui::ColorPanel* ui;

    Configuration m_config;
    std::set<Configuration> initializedConfigs;
    QMap<Configuration, int> mainSelectors;

    QStackedLayout* m_mainSelectorLayout;
};

#endif // COLORPANEL_H
