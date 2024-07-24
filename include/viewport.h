#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QScrollArea>
#include <QBoxLayout>
#include <QScrollBar>
#include <QShortcut>
#include "canvas.h"
#include "sessionmanager.h"

class Viewport : public QScrollArea
{
    Q_OBJECT
public:
    Viewport(QWidget *parent);

    virtual void showEvent(QShowEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;

    void setSourceImage(LayerImage *image);
    void setSessionManager(SessionManager* session);
    void updateView();

    void setZoom(int zoom);
    void zoomIn();
    void zoomOut();

    void centerView();
public:
    static const int MAX_MARGIN_SIZE = 900;
    static const int MAX_ZOOM = 10;
public slots:
    void onSessionDeleted() { m_session = nullptr; }
signals:
    void childMouseInputEnabled();
    void childMouseInputDisabled();
private:
    LayerImage* sourceImage() const { return m_session->sourceImage(); }
private:
    SessionManager* m_session = nullptr;
    QWidget m_background;
    QBoxLayout m_layout;
    Canvas m_canvas;

    double m_zoom;
    bool m_panMode = false;
    bool m_panning = false;
    bool m_wheelZoomMode = false;
    QPoint m_lastMousePosition;
};

#endif // VIEWPORT_H
