#include "layer.h"

Layer::Layer(QString name, QImage image, QObject* parent) :
    QObject{parent},
    m_name{name},
    m_image{image}
{

}

void Layer::setVisible(bool visible)
{
    if (m_visible == visible)
        return;

    m_visible = visible;
    emit layerChanged();
}

void Layer::toggleVisible()
{
    m_visible = !m_visible;
    emit layerChanged();
}

void Layer::fill(QRgb fillColor)
{
    m_image.fill(fillColor);
}
