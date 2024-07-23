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
private:
    // Session
    SessionManager* m_session = nullptr;

    // Settings
    QVector<QColor> m_penColors;
    int m_penWidth = 1;

    // Drawing
    QPointF m_lastPosition;
    QPointF m_currentPosition;
    QRect m_updateRegion;
    bool m_isDrawing = false;

    // debug
    std::chrono::high_resolution_clock::time_point m_drawStartTime;
    std::chrono::high_resolution_clock::time_point m_drawEndTime;
signals:
};

#endif // CANVAS_H
