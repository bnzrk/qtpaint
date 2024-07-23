#include "layerimage.h"

LayerImage::LayerImage(QSize size, QObject* parent) :
    QObject(parent),
    m_size{size}
{
    initializeImage();
}

LayerImage::~LayerImage()
{

}

void LayerImage::swapLayers(int a, int b)
{
    m_layers.swapItemsAt(a, b);
    emit imageChanged();
}

void LayerImage::initializeImage()
{
    createNewLayer();
}

void LayerImage::createNewLayer()
{
    QString name = "Layer ";
    name.append(QString::number(m_newLayerSuffix));
    QImage image = QImage(m_size, QImage::Format_ARGB32);
    if (m_layers.size() < 1)
    {
        image.fill(Qt::white);
    }
    else
    {
        image.fill(QColor(255, 255, 255, 0));
    }

    Layer* layer = new Layer(name, image, this);
    QObject::connect(layer, &Layer::layerChanged, this, &LayerImage::imageChanged);
    m_layers.push_back(layer);
    m_newLayerSuffix++;
    emit imageChanged();
}

void LayerImage::createNewLayer(int index)
{
    QString name = "Layer ";
    name.append(QString::number(m_newLayerSuffix));
    QImage image = QImage(m_size, QImage::Format_ARGB32);

    image.fill(QColor(255, 255, 255, 0));

    Layer* layer = new Layer(name, image, this);
    QObject::connect(layer, &Layer::layerChanged, this, &LayerImage::imageChanged);

    m_layers.insert(index, layer);

    m_newLayerSuffix++;
    emit imageChanged();
}

void LayerImage::removeLayer(int index)
{
    Layer* layer = m_layers.takeAt(index);
    delete layer;
    emit imageChanged();
}
