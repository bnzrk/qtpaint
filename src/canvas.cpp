#include "canvas.h"

Canvas::Canvas(QSize size, QObject* parent) :
    QObject(parent),
    m_size{size},
    m_zoom{1},
    m_undos{QStack<Command*>()},
    m_redos{QStack<Command*>()},
    m_primaryColor{Qt::red}
{
    initializeImage();
}

Canvas::~Canvas()
{
    for (auto c : m_undos)
        delete c;
    m_undos.clear();
    for (auto c : m_redos)
        delete c;
    m_redos.clear();
}

void Canvas::initializeImage()
{
    createNewLayer();
}

QPoint Canvas::snapPointToGrid(const QPoint &point) const
{
    int scale = pixelRatio();
    int x = (point.x() / scale) * scale;
    int y = (point.y() / scale) * scale;
    return QPoint(x, y);
}

QPoint Canvas::mapPointFromScaled(const QPoint &point) const
{
    QPoint adjusted = snapPointToGrid(point);
    int scale = pixelRatio();
    int x = adjusted.x() / scale;
    int y = point.y() / scale;
    return QPoint(x, y);
}

QRect Canvas::mapRectToScaled(const QRect &rect) const
{
    int scale = pixelRatio();

    QPoint topLeft = rect.topLeft() * scale;
    QPoint bottomRight = rect.bottomRight() * scale;

    return QRect(topLeft, bottomRight);
}

void Canvas::pushCommand(Command* command)
{
    if (!m_redos.empty())
        m_redos.clear();

    m_undos.push(command);
}

void Canvas::redo()
{
    if (m_redos.empty())
        return;

    Command* command = m_redos.pop();
    m_undos.push(command);

    command->execute();
}

void Canvas::undo()
{
    if (m_undos.empty())
        return;

    Command* command = m_undos.pop();
    m_redos.push(command);

    command->revert();
}

void Canvas::swapLayers(int a, int b)
{
    m_layers.swapItemsAt(a, b);
    emit canvasChanged();
}

void Canvas::createNewLayer()
{
    QString name = "Layer ";
    name.append(QString::number(m_newLayerSuffix));
    QImage image = QImage(m_size, QImage::Format_ARGB32);
    if (m_layers.size() < 1)
    {
        image.fill(Qt::white);
    }
    else
    {
        image.fill(QColor(255, 255, 255, 0));
    }

    Layer* layer = new Layer(name, image, this);
    QObject::connect(layer, &Layer::layerChanged, this, &Canvas::canvasChanged);
    m_layers.push_back(layer);
    m_newLayerSuffix++;
    emit canvasChanged();
}

void Canvas::createNewLayer(int index)
{
    QString name = "Layer ";
    name.append(QString::number(m_newLayerSuffix));
    QImage image = QImage(m_size, QImage::Format_ARGB32);

    image.fill(QColor(255, 255, 255, 0));

    Layer* layer = new Layer(name, image, this);
    QObject::connect(layer, &Layer::layerChanged, this, &Canvas::canvasChanged);

    m_layers.insert(index, layer);

    m_newLayerSuffix++;
    emit canvasChanged();
}

void Canvas::removeLayer(int index)
{
    Layer* layer = m_layers.takeAt(index);
    delete layer;
    emit canvasChanged();
}
