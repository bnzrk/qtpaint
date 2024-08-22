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
    createLayer(0, Qt::white);
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

int Canvas::indexOfLayer(Layer *layer)
{
    int index = m_layers.indexOf(layer);
    Q_ASSERT(index != -1);

    return index;
}

void Canvas::markDirtyFromLayer(const QRect &dirtyRegion)
{
    Layer* layer = qobject_cast<Layer*>(sender());
    Q_ASSERT(layer);

    int layerIndex = indexOfLayer(layer);
    markDirty(QVector<int>({layerIndex}), dirtyRegion);
}

void Canvas::markDirty(Layer *dirtyLayer, const QRect &dirtyRegion)
{
    int layerIndex = indexOfLayer(dirtyLayer);
    markDirty(QVector<int>({layerIndex}), dirtyRegion);
}

void Canvas::swapLayers(int a, int b)
{
    m_layers.swapItemsAt(a, b);
    emit canvasDirty(QVector<int>({a, b}), rect());
}

// FIXME: Calling on empty layer list is unsafe
void Canvas::createLayer(int index, const QColor &color)
{
    if (m_layers.size() < 1)
        Q_ASSERT(index == 0);

    QString name = "Layer ";
    name.append(QString::number(m_newLayerSuffix));
    QImage image = QImage(m_size, QImage::Format_ARGB32);
    image.fill(color);

    Layer* layer = new Layer(name, image, this);
    connect(layer, &Layer::layerDirty, this, &Canvas::markDirtyFromLayer);

    if (m_layers.size() < 1)
        m_layers.push_back(layer);
    else
        m_layers.insert(index, layer);
    m_newLayerSuffix++;

    emit layerInsersted(index);
    emit canvasDirty(QVector<int>({index}), rect());
}

// TODO: Add signal for layer removal and insertion
void Canvas::removeLayer(int index)
{
    Layer* layer = m_layers.takeAt(index);
    delete layer;
    layer = nullptr;

    emit canvasDirty(QVector<int>({index - 1}), rect());
}

void Canvas::markDirty(QVector<int> dirtyLayers, const QRect &dirtyRegion)
{
    emit canvasDirty(dirtyLayers, dirtyRegion);
}
void Canvas::setActiveLayer(int index)
{
    m_activeLayer = index;
}
