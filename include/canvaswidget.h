#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "canvas.h"
#include "pentool.h"

class CanvasWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CanvasWidget(QWidget* parent = nullptr);
    ~CanvasWidget();
    
    void setCanvas(Canvas* canvas);
public slots:
    void onMouseInputEnabled();
    void onMouseInputDisabled();
    void updateCanvasImage(const QVector<int>& dirtyLayers, const QRect& dirtyRegion);
private:
    // Event Overrides
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    // CanvasWidget
    double pixelRatio() const { return width() / m_canvas->size().width(); }

    // Mappings
    QRect snapRectToGrid(const QRect& rect) const;
    QPoint snapPointToGrid(const QPoint& point) const;
    QRect fitRectToPixel(const QRect& rect) const;

    QPoint mapPointToImage(const QPoint& point) const;
    QPoint mapPointToWidget(const QPoint& point) const;
    QRect mapRectToImage(const QRect& rect) const;
    QRect mapRectToWidget(const QRect& rect) const;
private:
    QPixmap generateBackgroundPattern(const QSize &gridSize=QSize(8, 8), const QColor &backgroundColor=QColor(234, 234, 234), const QColor &foregroundColor=QColor(181, 181, 181)) const;
private:
    // Canvas
    Canvas* m_canvas;

    // Settings
    Tool* m_tool;
    QPixmap m_background;

    // Input
    bool m_acceptMouseInput = true;
};

#endif // CANVASWIDGET_H
