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
    for (int i = 0; i < 3; i++)
    {
        createNewLayer();
    }
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
