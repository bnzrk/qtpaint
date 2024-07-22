#ifndef LAYERPANEL_H
#define LAYERPANEL_H

#include <QWidget>
#include "layerimage.h"
#include "layerpanelitem.h"

namespace Ui
{
class LayerPanel;
}

class LayerPanel : public QWidget
{
    Q_OBJECT

public:
    explicit LayerPanel(QWidget* parent = nullptr);
    ~LayerPanel();

    void setSourceImage(LayerImage* sourceImage);
public slots:
    void onVisibilityToggled(LayerPanelItem* sender);
    void onMoveUpRequested(LayerPanelItem* sender);
    void onMoveDownRequested(LayerPanelItem* sender);
private:
    void swapLayers(int a, int b);
    void toggleLayerVisibility(int index);

    void populateItems();
    void addItem(const Layer& reference);
    void updateItemDisplay(int index);

    int getIndexOf(const LayerPanelItem* item) const;
    LayerPanelItem* getItemAtIndex(int index);
private:
    Ui::LayerPanel* ui;

    LayerImage* m_sourceImage = nullptr;
    QVector<LayerPanelItem*> m_items;
};

#endif // LAYERPANEL_H
