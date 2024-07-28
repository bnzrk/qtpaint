#include "strokecommand.h"

StrokeCommand::StrokeCommand(Canvas* canvas, int targetLayer, Stroke stroke) :
    PaintCommand(canvas, QVector<int>({targetLayer})),
    m_stroke{stroke},
    m_buffer{QImage(m_stroke.boundingRect().size(), QImage::Format_ARGB32)}
{
    // Store backup of affected region in buffer
    QPainter painter(&m_buffer);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(m_buffer.rect(), *m_canvas->layerAt(targetLayer)->image(), m_stroke.boundingRect());
}

void StrokeCommand::execute() const
{
    // Paint stroke
    QPainter painter(m_canvas->layerAt(targetLayer())->image());
    painter.setPen(m_stroke.pen());
    painter.drawPath(m_stroke.path());
    painter.end();

    // TEMP
    emit m_canvas->canvasImageChanged(m_targetLayers, affectedRegion());
}

void StrokeCommand::revert() const
{
    // Repaint affected region from buffer
    QPainter painter(m_canvas->layerAt(targetLayer())->image());
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(m_stroke.boundingRect(), m_buffer, m_buffer.rect());

    // TEMP
    emit m_canvas->canvasImageChanged(m_targetLayers, affectedRegion());
}
