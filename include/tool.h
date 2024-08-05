#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QMouseEvent>
#include <QCursor>
#include "canvas.h"

/*
 * Abstract class for canvas tools.
 */
class Tool : public QObject
{
    Q_OBJECT
public:
    explicit Tool(QString name, QString icon, Canvas* canvas) : QObject{nullptr}, m_name{name}, m_icon{icon}, m_canvas{canvas} {}
    virtual ~Tool() {}

    virtual void mousePressEvent(QMouseEvent* event) = 0;
    virtual void mouseMoveEvent(QMouseEvent* event) = 0;
    virtual void mouseReleaseEvent(QMouseEvent* event) = 0;

    Canvas* canvas() const { return m_canvas; }
    virtual QCursor cursor() const = 0;
protected:
    QString m_name;
    QString m_icon;

    Canvas* m_canvas;
signals:
    void cursorChanged(QCursor cursor);
};

#endif // TOOL_H
