#include "../include/pentool.h"

PenTool::PenTool(Canvas* canvas) :
    Tool("Pen", "", canvas),
    m_stroke{Stroke(QPen())},
    m_drawing{false},
    m_pen{QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)},
    m_targetLayer{0}
{
}

void PenTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_drawing)
    {
        QPoint point = m_canvas->mapPointFromScaled(event->pos());

        m_drawing = true;
        m_targetLayer = m_canvas->activeLayer();

        Layer* target = m_canvas->layerAt(m_targetLayer);
        target->enableBuffer();

        m_stroke = Stroke(m_pen);
        m_stroke.addPoint(point);

        QPainter painter(target->image());
        painter.setPen(m_pen);
        painter.drawPoint(point);

        // TODO: Remove external emit
        emit m_canvas->canvasImageChanged(m_targetLayer, m_stroke.boundingRect());
    }
}

void PenTool::mouseMoveEvent(QMouseEvent *event)
{
    if (m_drawing)
    {
        QPoint point = m_canvas->mapPointFromScaled(event->pos());

        Layer* target = m_canvas->layerAt(m_targetLayer);

        m_stroke.addPoint(point);
        QPoint prev = m_stroke.previous();

        QPainter painter(target->image());
        painter.setPen(m_pen);
        painter.drawLine(prev, point);

        // TODO: Remove external emit
        emit m_canvas->canvasImageChanged(m_targetLayer, m_stroke.boundingRect());
    }
}

void PenTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_drawing)
    {
        m_drawing = false;

        // Finalize stroke and push paint command
        Layer* target = m_canvas->layerAt(m_targetLayer);
        QImage original = target->finalizeBuffer();
        PaintInfo info = PaintInfo(original, *target->image(), m_stroke.boundingRect());
        m_canvas->pushCommand(new PaintLayerCommand(m_canvas, m_targetLayer, info));
    }
}

void PenTool::setSize(int size)
{
    m_pen.setWidth(size);
}

QCursor PenTool::cursor() const
{
    return QCursor(Qt::ArrowCursor);
}

