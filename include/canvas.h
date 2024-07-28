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

    void initializeImage();

    int activeLayer() const { return m_activeLayer; }
    void setActiveLayer(int index) { m_activeLayer = index; }

    void createNewLayer();
    void createNewLayer(int index);
    void removeLayer(int index);
    void swapLayers(int a, int b);

    void toggleLayerVisible(int index);

    QVector<Layer*> layers() { return m_layers; }
    int layerCount() const { return m_layers.size(); }
    Layer* layerAt(int index) { return m_layers[index]; }
    QSize size() const { return m_size; }

    void pushCommand(Command* command);
    void redo();
    void undo();
signals:
    void canvasChanged();
    void canvasImageChanged(QVector<int> dirtyLayers, QRect diryRegion);
private:
    QSize m_size;

    QVector<Layer*> m_layers;
    int m_activeLayer = 0;
    int m_newLayerSuffix = 1;

    QStack<Command*> m_undos;
    QStack<Command*> m_redos;
};

#endif // CANVAS_H
