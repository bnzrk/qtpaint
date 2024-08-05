#include "layer.h"

Layer::Layer(QString name, QImage image, QObject* parent) :
    PaintImage{image, parent},
    m_name{name}
{

}

void Layer::markDirty(const QRect& dirtyRegion)
{
    emit layerImageChanged(dirtyRegion);
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
    emit layerChanged();
}

void Layer::clear()
{
    fill(qRgba(0, 0, 0, 0));
    emit layerChanged();
}
