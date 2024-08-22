#include "layer.h"

Layer::Layer(QString name, QImage image, QObject* parent) :
    PaintImage{image, parent},
    m_name{name},
    m_blendMode{BlendMode::Normal}
{
    m_size = image.size();
}

void Layer::markDirty(const QRect& dirtyRegion)
{
    emit layerDirty(dirtyRegion);
}

void Layer::setVisible(bool visible)
{
    if (m_visible == visible)
        return;

    m_visible = visible;
    emit layerDirty(rect());
}

void Layer::toggleVisible()
{
    m_visible = !m_visible;
    emit layerDirty(rect());
}

void Layer::fill(QRgb fillColor)
{
    m_image.fill(fillColor);
    emit layerDirty(rect());
}

void Layer::clear()
{
    fill(qRgba(0, 0, 0, 0));
    emit layerDirty(rect());
}

void Layer::setBlendMode(BlendMode blendMode)
{
    m_blendMode = blendMode;
    emit layerDirty(rect());
}
