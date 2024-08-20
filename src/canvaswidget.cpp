#include "CanvasWidget.h"

CanvasWidget::CanvasWidget(QWidget* parent) :
    QWidget{parent}
{
    // setup widget
    setMouseTracking(false);
    hide();
}

CanvasWidget::~CanvasWidget()
{
}

void CanvasWidget::setCanvas(Canvas* canvas)
{
    m_canvas = canvas;

    if (!m_canvas)
    {
        setMouseTracking(false);
        hide();
        return;
    }

    m_tool = new PenTool(m_canvas);

    QObject::connect(m_canvas, &Canvas::canvasImageChanged, this, &CanvasWidget::onCanvasImageChanged);
    setFixedSize(canvas->size());

    // generate background
    m_background = QPixmap(canvas->size());
    QPainter painter(&m_background);
    QBrush brush;
    brush.setTexture(generateBackgroundPattern());
    painter.fillRect(m_background.rect(), brush);

    setMouseTracking(true);
    show();
}

void CanvasWidget::onCanvasImageChanged(int dirtyLayer, QRect dirtyRegion)
{
    QRect repaintRegion = m_canvas->mapRectToScaled(dirtyRegion);
    repaintRegion = fitRectToPixel(repaintRegion);
    repaint(repaintRegion);
}

void CanvasWidget::onMouseInputEnabled()
{
    m_acceptMouseInput = true;
}

void CanvasWidget::onMouseInputDisabled()
{
    m_acceptMouseInput = false;
}

void CanvasWidget::mousePressEvent(QMouseEvent* event)
{
    if (m_acceptMouseInput)
    {
        if (event->button() == Qt::RightButton) // TEMP
        {
            m_canvas->setPrimaryColor((m_canvas->primaryColor() == Qt::red) ? Qt::green : Qt::red);
            return;
        }
        if (m_tool)
        {
            m_tool->mousePressEvent(event);
        }
    }
    event->ignore();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_acceptMouseInput)
    {
        if (m_tool)
        {
            m_tool->mouseMoveEvent(event);
        }
    }
    event->ignore();
}

void CanvasWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_acceptMouseInput)
    {
        if (m_tool)
        {
            m_tool->mouseReleaseEvent(event);
            update();
        }
    }
    event->ignore();
}

// Generates a grid pattern using the given grid size and colors.
QPixmap CanvasWidget::generateBackgroundPattern(const QSize &gridSize, const QColor &backgroundColor, const QColor &foregroundColor) const
{
    QPixmap pattern = QPixmap(gridSize * 2);
    QPainter painter(&pattern);
    painter.setPen(Qt::NoPen);
    painter.setBrush(backgroundColor);
    painter.drawRect(0, 0, 8, 8);
    painter.drawRect(8, 8, 8, 8);
    painter.setBrush(foregroundColor);
    painter.drawRect(8, 0, 8, 8);
    painter.drawRect(0, 8, 8, 8);
    painter.end();
    return pattern;
}

// Snaps the given position to the displayed pixel grid.
QPoint CanvasWidget::snapPointToGrid(const QPoint& point) const
{
    int scale = pixelRatio();
    int x = (point.x() / scale) * scale;
    int y = (point.y() / scale) * scale;
    return QPoint(x, y);
}

// Fits a rect to the displayed pixels.
QRect CanvasWidget::fitRectToPixel(const QRect& rect) const
{
    double scale = pixelRatio();
    int left = std::floor(rect.left() / scale) * scale;
    int top = std::floor(rect.top() / scale) * scale;
    int right = std::ceil((rect.right() + 1) / scale) * scale - 1;
    int bottom = std::ceil((rect.bottom() + 1) / scale) * scale - 1;
    return QRect(QPoint(left, top), QPoint(right, bottom));
}

// Maps a point from the widget to the cooresponding point in unscaled space.
QPoint CanvasWidget::mapPointToImage(const QPoint& point) const
{
    int scale = pixelRatio();
    int x = point.x() / scale;
    int y = point.y() / scale;
    return QPoint(x, y);
}

// TODO: rewrite to be compatbile with scale < 1
// Maps a rect from the CanvasWidget widget to the CanvasWidget image.
QRect CanvasWidget::mapRectToImage(const QRect& rect) const
{
    QPoint topLeft = mapPointToImage(rect.topLeft());
    QPoint bottomRight = mapPointToImage(rect.bottomRight());

    return QRect(topLeft, bottomRight);
}

// TODO: rewrite to be compatbile with scale < 1
// Maps a rect from the CanvasWidget image to the CanvasWidget widget.
QRect CanvasWidget::mapRectToWidget(const QRect& rect) const
{
    int scale = pixelRatio();

    QPoint topLeft = rect.topLeft() * scale;
    QPoint bottomRight = rect.bottomRight() * scale;

    return QRect(topLeft, bottomRight);
}

void CanvasWidget::paintEvent(QPaintEvent* event)
{
    if (!m_canvas)
        return;

    QPainter painter(this);

    // expand paint region to fit widget pixel grid
    QRect paintRegion = fitRectToPixel(event->rect());
    QRect imageRegion = mapRectToImage(paintRegion);

    // draw background and canvas layers from paint region
    painter.drawPixmap(paintRegion, m_background, imageRegion);
    for (int i = 0; i < m_canvas->layerCount(); i++)
    {
        if (m_canvas->layerAt(i)->isVisible())
        {
            painter.setCompositionMode((QPainter::CompositionMode)m_canvas->layerAt(i)->blendMode());
            painter.drawImage(paintRegion, *(m_canvas->layerAt(i)->image()), imageRegion);
        }
    }
    painter.end();
}
