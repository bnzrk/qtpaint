#ifndef LAYERPANEL_H
#define LAYERPANEL_H

#include <QWidget>
#include "layerimage.h"
#include "layerpanelitem.h"
#include "sessionmanager.h"

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
    void setSessionManager(SessionManager* session);
public slots:
    void onSessionDeleted();
    void onVisibilityToggled(LayerPanelItem* sender);
    void onMoveUpRequested(LayerPanelItem* sender);
    void onMoveDownRequested(LayerPanelItem* sender);
    void onItemClicked(LayerPanelItem* sender);
private:
    void swapLayers(int a, int b);
    void toggleLayerVisibility(int index);

    void populateItems();
    void addItem(const Layer& reference);
    void updateItemDisplay(int index);

    int getIndexOf(const LayerPanelItem* item) const;
    LayerPanelItem* getItemAtIndex(int index);

    LayerImage* sourceImage() const { return m_session->sourceImage(); }
private:
    Ui::LayerPanel* ui;

    SessionManager* m_session = nullptr;
    QVector<LayerPanelItem*> m_items;
};

#endif // LAYERPANEL_H
