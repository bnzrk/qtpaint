#include "../include/paintlayercommand.h"

PaintLayerCommand::PaintLayerCommand(Canvas *canvas, int targetLayer, PaintInfo paintInfo) :
    PaintCommand{canvas, QVector<int>({targetLayer})},
    m_paintInfo{paintInfo}
{
}

void PaintLayerCommand::execute() const
{
    // Repaint affected region
    QPainter painter(m_canvas->layerAt(targetLayer())->image());
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(affectedRegion(), m_paintInfo.modified(), m_paintInfo.modified().rect());

    m_canvas->layerAt(targetLayer())->markDirty(affectedRegion());
}

void PaintLayerCommand::revert() const
{
    // Repaint affected region
    QPainter painter(m_canvas->layerAt(targetLayer())->image());
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(affectedRegion(), m_paintInfo.original(), m_paintInfo.original().rect());

    m_canvas->layerAt(targetLayer())->markDirty(affectedRegion());
}
