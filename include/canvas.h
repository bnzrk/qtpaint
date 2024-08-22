#ifndef CANVAS_H
#define CANVAS_H

#include <QtCore/QObject>
#include <QImage>
#include <QDebug>
#include <QPainter>
#include <QStack>
#include "command.h"
#include "layer.h"

class Canvas : public QObject
{
    Q_OBJECT
public:
    explicit Canvas(QSize size, QObject* parent = nullptr);
    explicit Canvas(int width, int height, QObject* parent = nullptr) : Canvas(QSize(width, height)) {}
    ~Canvas();

    // TEMP
    QColor primaryColor() { return m_primaryColor; }
    void setPrimaryColor(const QColor& color) { m_primaryColor = color; }

    // Canvas
    void initializeImage();
    QRect rect() const { return QRect(QPoint(0, 0), m_size); }
    QSize size() const { return m_size; }

    double pixelRatio() const { return (m_size.width() * m_zoom) / size().width(); }
    QPoint snapPointToGrid(const QPoint& point) const;
    QPoint mapPointFromScaled(const QPoint& point) const;
    QRect mapRectToScaled(const QRect& rect) const;

    // Zoom
    double zoom() const { return m_zoom; }
    void setZoom(int zoom) { m_zoom = zoom; }

    // Layers
    int activeLayer() const { return m_activeLayer; }
    void setActiveLayer(int index);
    void createLayer(int index=0, const QColor& color=Qt::transparent);
    void removeLayer(int index);
    void swapLayers(int a, int b);
    void toggleLayerVisible(int index);
    QVector<Layer*> layers() { return m_layers; }
    int layerCount() const { return m_layers.size(); }
    Layer* layerAt(int index) { return m_layers[index]; }

    // Command History
    void pushCommand(Command* command);
    void redo();
    void undo();
private:
    int indexOfLayer(Layer* layer);
public slots:
    void markDirty(Layer* dirtyLayer, const QRect& dirtyRegion);
    void markDirty(QVector<int> dirtyLayers, const QRect& dirtyRegion);
private slots:
    void markDirtyFromLayer(const QRect& dirtyRegion);
signals:
    void canvasDirty(const QVector<int>& dirtyLayers, const QRect& dirtyRegion);
    void layerInsersted(int index);
private:
    // TEMP
    QColor m_primaryColor;

    QSize m_size;
    double m_zoom;

    QVector<Layer*> m_layers;
    int m_activeLayer = 0;
    int m_newLayerSuffix = 1;

    QStack<Command*> m_undos;
    QStack<Command*> m_redos;
};

#endif // CANVAS_H
