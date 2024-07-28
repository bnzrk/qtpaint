#ifndef STROKE_H
#define STROKE_H

#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QRect>

/*
 * Holds data for a stroke such as its pen, path, bounding rect, etc.
 */
class Stroke
{
public:
    Stroke(QPen pen);

    void setPen(QPen pen) { m_pen = pen; }
    QPen pen() const { return m_pen; }
    QPainterPath path() const { return m_path; }
    void addPoint(const QPoint &point);
    QRect boundingRect() const;
private:
    QPen m_pen;
    QPainterPath m_path;
};

#endif // STROKE_H
