#ifndef CANVASVIEWPORT_H
#define CANVASVIEWPORT_H

#include <QScrollArea>
#include <QBoxLayout>
#include <QScrollBar>
#include <QShortcut>
#include "canvaswidget.h"

class CanvasViewport : public QScrollArea
{
    Q_OBJECT
public:
    CanvasViewport(QWidget *parent);
    ~CanvasViewport();

    virtual void showEvent(QShowEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

    void setCanvas(Canvas* canvas);
    void updateView();

    void setZoom(int zoom);
    void zoomIn();
    void zoomOut();

    void centerView();
public:
    static const int MAX_MARGIN_SIZE = 2000;
    static const int MAX_ZOOM = 10;
signals:
    void childMouseInputEnabled();
    void childMouseInputDisabled();
private:
    Canvas* m_canvas;
    CanvasWidget* m_canvasWidget;
    QWidget* m_background;
    QBoxLayout* m_layout;

    double m_zoom;
    bool m_panMode = false;
    bool m_panning = false;
    bool m_wheelZoomMode = false;
    QPoint m_lastMousePosition;
};

#endif // CANVASVIEWPORT_H
