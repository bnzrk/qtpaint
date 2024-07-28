#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "canvas.h"
#include "strokecommand.h"
#include "stroke.h"

class CanvasWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CanvasWidget(QWidget* parent = nullptr);
    ~CanvasWidget();
    
    void setCanvas(Canvas* canvas);
    bool drawing() const { return m_isDrawing; }
public slots:
    void onMouseInputEnabled();
    void onMouseInputDisabled();
    void onCanvasChanged() { update(); }
    void onCanvasImageChanged(QVector<int> dirtyLayers, QRect dirtyRegion);
private:
    // Event Overrides
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    // CanvasWidget
    void draw();
    double pixelRatio() const { return width() / m_canvas->size().width(); }

    // Mappings
    QRect snapRectToGrid(const QRect& rect) const;
    QPoint snapPointToGrid(const QPoint& point) const;
    QRect fitRectToPixel(const QRect& rect) const;

    QPoint mapPointToImage(const QPoint& point) const;
    QPoint mapPointToWidget(const QPointF& point) const;
    QRect mapRectToImage(const QRect& rect) const;
    QRect mapRectToWidget(const QRect& rect) const;
private:
    QPixmap generateBackgroundPattern(const QSize &gridSize=QSize(8, 8), const QColor &backgroundColor=QColor(234, 234, 234), const QColor &foregroundColor=QColor(181, 181, 181)) const;
private:
    // Canvas
    Canvas* m_canvas;
    Stroke m_stroke;

    // Settings
    QVector<QColor> m_penColors;
    int m_penWidth = 1;
    QPixmap m_background;

    // Input
    bool m_acceptMouseInput = true;

    // Drawing
    QPointF m_lastMousePosition;
    QPointF m_currentMousePosition;
    QRect m_updateRegion;
    bool m_isDrawing = false;
};

#endif // CANVASWIDGET_H
