#include "canvas.h"

Canvas::Canvas(QWidget* parent) : QWidget{parent}, m_sourceImage{nullptr}
{
    // setup widget
    setMouseTracking(true);
    setFixedSize(800, 600);

    // setup pen
    m_penColors = { Qt::black, Qt::red, Qt::blue };
    m_penWidth = 3;

    // setup background
    m_background = QImage(this->size(), QImage::Format_ARGB32);
    m_background.fill(QColor(32, 33, 36, 255));
}

Canvas::~Canvas()
{

}

void Canvas::setSourceImage(LayerImage *image)
{
    m_sourceImage = image;

    QObject::connect(m_sourceImage, &LayerImage::imageChanged, this, &Canvas::onImageChanged);
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton && !m_isDrawing)
    {
        m_isDrawing = true;
        m_lastPosition = m_currentPosition = event->position();
        draw();
    }
    else if(event->button() == Qt::MiddleButton)
    {
        m_activeLayer = (m_activeLayer + 1) % m_sourceImage->layers().size();
        qDebug() << "current layer: " << m_sourceImage->getLayerAtIndex(m_activeLayer)->getName();
    }
    event->ignore();
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if(m_isDrawing)
    {
        m_currentPosition = event->position();
        if (m_lastPosition != m_currentPosition)
            draw();
        m_lastPosition = m_currentPosition;
    }
    event->ignore();
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton && m_isDrawing)
    {
        m_currentPosition = event->position();
        draw();
        m_isDrawing = false;
        update();
    }
    event->ignore();
}

QPoint Canvas::snapPointToGrid(const QPoint& point) const
{
    int scale = pixelRatio();
    int x = (point.x() / scale) * scale;
    int y = (point.y() / scale) * scale;
    return QPoint(x, y);
}

// Fits a rect to the displayed pixels.
QRect Canvas::fitRectToPixel(const QRect& rect) const
{
    double scale = pixelRatio();
    int left = std::floor(rect.left() / scale) * scale;
    int top = std::floor(rect.top() / scale) * scale;
    int right = std::ceil((rect.right() + 1) / scale) * scale - 1;
    int bottom = std::ceil((rect.bottom() + 1) / scale) * scale - 1;
    return QRect(QPoint(left, top), QPoint(right, bottom));
}

// Maps a point from the widget to the cooresponding point in unscaled space.
QPoint Canvas::mapPointToImage(const QPoint& point) const
{
    int scale = pixelRatio();
    int x = point.x() / scale;
    int y = point.y() / scale;
    return QPoint(x, y);
}

// TODO: rewrite to be compatbile with scale < 1
// Maps a rect from the canvas widget to the canvas image.
QRect Canvas::mapRectToImage(const QRect& rect) const
{
    QPoint topLeft = mapPointToImage(rect.topLeft());
    QPoint bottomRight = mapPointToImage(rect.bottomRight());

    return QRect(topLeft, bottomRight);
}

// TODO: rewrite to be compatbile with scale < 1
// Maps a rect from the canvas image to the canvas widget.
QRect Canvas::mapRectToWidget(const QRect& rect) const
{
    int scale = pixelRatio();

    QPoint topLeft = rect.topLeft() * scale;
    QPoint bottomRight = rect.bottomRight() * scale;

    return QRect(topLeft, bottomRight);
}

// Draws a line from the previous mouse position to the current mouse position on the active canvas layer and updates the affected widget region.
void Canvas::draw()
{
    int scale = pixelRatio();

    // snap mouse positions to points on widget pixel grid
    QPoint prevOnWidget = snapPointToGrid(m_lastPosition.toPoint());
    QPoint curOnWidget = snapPointToGrid(m_currentPosition.toPoint());

    // map widget points to image points
    QPoint prevOnImage = mapPointToImage(prevOnWidget);
    QPoint curOnImage = mapPointToImage(curOnWidget);

    // calculate update region from widget points and expand by pen width
    int x = std::min(prevOnWidget.x(), curOnWidget.x());
    int y = std::min(prevOnWidget.y(), curOnWidget.y());
    int w = std::abs(curOnWidget.x() - prevOnWidget.x()) + scale;
    int h = std::abs(curOnWidget.y() - prevOnWidget.y()) + scale;
    m_updateRegion = QRect(x, y, w, h);
    int adjustFactor = m_penWidth * scale;
    m_updateRegion.adjust(-adjustFactor, -adjustFactor, adjustFactor, adjustFactor);

    // setup pen and draw between image points
    QColor color = m_penColors[m_activeLayer % m_penColors.size()];
    QPen pen(color, m_penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPainter painter;
    painter.begin(m_sourceImage->getLayerAtIndex(m_activeLayer)->getImage());
    painter.setPen(pen);
    painter.drawLine(prevOnImage, curOnImage);
    painter.end();

    // repaint affected region of widget
    repaint(m_updateRegion);
}

void Canvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // expand paint region to fit widget pixel grid
    QRect paintRegion = fitRectToPixel(event->rect());
    QRect imageRegion = mapRectToImage(paintRegion);

    // draw canvas layers from paint region
    for (int i = 0; i < m_sourceImage->getLayerCount(); i++)
    {
        if (m_sourceImage->getLayerAtIndex(i)->isVisible())
            painter.drawImage(paintRegion, *(m_sourceImage->getLayerAtIndex(i)->getImage()), imageRegion);
    }
    painter.end();

    // reset update region
    m_updateRegion = QRect();
}
