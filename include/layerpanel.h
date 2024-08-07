#ifndef LAYERPANEL_H
#define LAYERPANEL_H

#include <QWidget>
#include <QScrollBar>
#include <QFile>
#include "canvas.h"
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
    void cleanup();

    virtual void showEvent(QShowEvent* event) override;

    void setCanvas(Canvas* canvas);
public slots:
    void onVisibilityToggled(LayerPanelItem* sender);
    void onMoveUpRequested(LayerPanelItem* sender);
    void onMoveDownRequested(LayerPanelItem* sender);
    void onItemClicked(LayerPanelItem* sender);
    void onNewLayerClicked();
    void onRemoveLayerClicked();
private:
    void swapLayers(int a, int b);
    void toggleLayerVisibility(int index);

    void populateItems();
    void addItem(const Layer& reference);
    void removeItem(int index);
    void updateItemDisplay(int index);

    int getIndexOf(const LayerPanelItem* item) const;
    LayerPanelItem* getItemAtIndex(int index);
private:
    Ui::LayerPanel* ui;

    Canvas* m_canvas;
    QVector<LayerPanelItem*> m_items;
};

#endif // LAYERPANEL_H
