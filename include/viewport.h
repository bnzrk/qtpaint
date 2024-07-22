#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QScrollArea>
#include <QBoxLayout>
#include <QScrollBar>
#include <QShortcut>
#include "canvas.h"

class Viewport : public QScrollArea
{
    Q_OBJECT
public:
    Viewport(QWidget *parent);

    virtual void showEvent(QShowEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;

    void setSourceImage(LayerImage *image);
    void updateDisplay();

    void setZoom(int zoom);
    void zoomIn();
    void zoomOut();

    void centerView();
public:
    static const int MAX_MARGIN_SIZE = 900;
    static const int MAX_ZOOM = 10;
private:
    LayerImage *m_sourceImage = nullptr;
    QWidget m_background;
    QBoxLayout m_layout;
    Canvas m_canvas;

    double m_zoom;
};

#endif // VIEWPORT_H
