#include "canvas.h"

Canvas::Canvas(QSize size, QObject* parent) :
    QObject(parent),
    m_size{size},
    m_undos{QStack<Command*>()},
    m_redos{QStack<Command*>()}
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

void Canvas::pushCommand(Command* command)
{
    if (!m_redos.empty())
        m_redos.clear();

    m_redos.push(command);
    redo();
}

void Canvas::redo()
{
    if (m_redos.empty())
        return;

    Command* command = m_redos.pop();
    m_undos.push(command);

    command->execute();
    //emit canvasChanged();
}

void Canvas::undo()
{
    if (m_undos.empty())
        return;

    Command* command = m_undos.pop();
    m_redos.push(command);

    command->revert();
    //emit canvasChanged();
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
