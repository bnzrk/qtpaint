#ifndef LAYERIMAGE_H
#define LAYERIMAGE_H

#include <QtCore/QObject>
#include <QImage>
#include <QDebug>
#include <QPainter>
#include "layer.h"

class LayerImage : public QObject
{
    Q_OBJECT
public:
    explicit LayerImage(QSize size, QObject* parent = nullptr);
    explicit LayerImage(int width, int height, QObject* parent = nullptr) : LayerImage(QSize(width, height)) {}
    ~LayerImage();

    void initializeImage();

    void createNewLayer();
    void createNewLayer(int index);
    void removeLayer(int index);
    void swapLayers(int a, int b);
    void toggleLayerVisible(int index);

    QVector<Layer*> layers() { return m_layers; }
    int layerCount() const { return m_layers.size(); }
    Layer* layerAtIndex(int index) { return m_layers[index]; }
    QSize size() const { return m_size; }
signals:
    void imageChanged();
private:
    QSize m_size;
    QVector<Layer*> m_layers;
    int m_newLayerSuffix = 1;
};

#endif // LAYERIMAGE_H
