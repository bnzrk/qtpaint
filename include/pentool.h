#ifndef PENTOOL_H
#define PENTOOL_H

#include <QPen>
#include "tool.h"
#include "stroke.h"
#include "paintlayercommand.h"

class PenTool : public Tool
{
    Q_OBJECT
public:
    explicit PenTool(Canvas* canvas);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    bool drawing() { return m_drawing; }
    QCursor cursor() const;
    int size() { return m_pen.width(); }
public slots:
    void setSize(int size);
private:
    int m_targetLayer;
    QPen m_pen;
    Stroke m_stroke;
    bool m_drawing;
};

#endif // PENTOOL_H
