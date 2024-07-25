#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include "layerimage.h"
#include "sessionmanager.h"

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget* parent = nullptr);
    ~Canvas();
    
    void setSessionManager(SessionManager* session);
    bool drawing() const { return m_isDrawing; }
public slots:
    void onImageChanged() { update(); }
    void onSessionDeleted();
private:
    // Event Overrides
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;

    // Canvas
    void draw();
    double pixelRatio() const { return width() / sourceImage()->size().width(); }

    // Mappings
    QRect snapRectToGrid(const QRect& rect) const;
    QPoint snapPointToGrid(const QPoint& point) const;
    QRect fitRectToPixel(const QRect& rect) const;

    QPoint mapPointToImage(const QPoint& point) const;
    QPoint mapPointToWidget(const QPointF& point) const;
    QRect mapRectToImage(const QRect& rect) const;
    QRect mapRectToWidget(const QRect& rect) const;

    LayerImage* sourceImage() const { return m_session->sourceImage(); }
public slots:
    void onMouseInputEnabled();
    void onMouseInputDisabled();
private:
    QPixmap generateBackgroundPattern(const QSize &gridSize=QSize(8, 8), const QColor &backgroundColor=QColor(234, 234, 234), const QColor &foregroundColor=QColor(181, 181, 181)) const;
private:
    // Session
    SessionManager* m_session = nullptr;

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

    // debug
    std::chrono::high_resolution_clock::time_point m_drawStartTime;
    std::chrono::high_resolution_clock::time_point m_drawEndTime;
};

#endif // CANVAS_H
