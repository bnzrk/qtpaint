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
    inline static const QList<QString> BLEND_MODE_STRINGS = {"Normal",
                                                             "Multiply",
                                                             "Add",
                                                             "Color Burn",
                                                             "Color Dodge",
                                                             "Screen",
                                                             "Overlay",
                                                             "Lighten",
                                                             "Darken",
                                                             "Hard Light",
                                                             "Soft Light",
                                                             "Difference",
                                                             "Exclusion",
                                                             "Xor"};
public:
    explicit LayerPanel(QWidget* parent = nullptr);
    ~LayerPanel();

    void showEvent(QShowEvent* event) override;

    void setCanvas(Canvas* canvas);
    void cleanup();
public slots:
    void toggleLayerVisibility();
    void moveLayerUp();
    void moveLayerDown();
    void changeActiveLayer();
    void createNewLayer();
    void removeActiveLayer();
    void changeLayerBlendMode(const QString &text);

    void updateItem(int index);
    void updateItems();
    void updateItemsInRange(int start, int end);
    void swapItems(int a, int b);
    void insertItem(int index);
    void removeItem(int index);
    void changeActiveItem(int oldIndex, int newIndex);
private:
    void populateItems();
    int indexOfItem(const LayerPanelItem* item) const;
    LayerPanelItem* itemAt(int index);

    QString blendModeName(Layer::BlendMode blendMode) const;
    Layer::BlendMode blendModeFromName(const QString& name);
private:
    Ui::LayerPanel* ui;

    Canvas* m_canvas;
    QVector<LayerPanelItem*> m_items;
};

#endif // LAYERPANEL_H
