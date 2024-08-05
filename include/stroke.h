#ifndef STROKE_H
#define STROKE_H

#include <QPainter>
#include <QPainterPath>
#include <QPainterPathStroker>
#include <QRect>

/*
 * Holds information about a stroke such as its pen, path, bounding rect, etc.
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
    QPoint previous() const { return m_previous; }
    QPoint current() const { return m_current; }
private:
    // Path
    QPainterPath m_path;
    QPoint m_previous;
    QPoint m_current;

    // Settings
    QPen m_pen;


};

#endif // STROKE_H
