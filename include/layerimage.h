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
    explicit LayerImage(QObject* parent = nullptr);
    ~LayerImage();

    void initializeImage(QSize size);
    void initializeImage(int width, int height) { initializeImage(QSize(width, height)); }

    void createNewLayer();
    void swapLayers(int a, int b);
    void toggleLayerVisible(int index);

    QVector<Layer*> layers() { return m_layers; }
    int getLayerCount() { return m_layers.size(); }
    Layer* getLayerAtIndex(int index) { return m_layers[index]; }
    QSize getSize() { return m_size; }
signals:
    void imageChanged();
private:
    QSize m_size;
    QVector<Layer*> m_layers;
    int m_newLayerSuffix = 1;
};

#endif // LAYERIMAGE_H
