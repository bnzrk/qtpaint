#include "../include/stroke.h"

Stroke::Stroke(QPen pen) :
    m_pen{pen},
    m_path{QPainterPath()}
{
}

void Stroke::addPoint(const QPoint &point)
{
    m_previous = m_current;
    if (m_path.capacity() < 1)
        m_path.moveTo(point);
    else
    {
        m_path.lineTo(point);
    }
    m_current = point;
}

QRect Stroke::boundingRect() const
{
    QRect bounds = m_path.boundingRect().toRect();
    int adjustFactor = std::ceil(m_pen.width() * 0.5);
    bounds.adjust(-adjustFactor, -adjustFactor, adjustFactor, adjustFactor);
    return bounds;
}


