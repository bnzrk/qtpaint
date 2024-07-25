#include "canvas.h"

Canvas::Canvas(QWidget* parent) :
    QWidget{parent}
{
    // setup widget
    setMouseTracking(false);
    hide();

    // setup pen
    m_penColors = { Qt::black, Qt::red, Qt::blue };
    m_penWidth = 3;
}

Canvas::~Canvas()
{

}

void Canvas::onSessionDeleted()
{
    m_session = nullptr;
    setMouseTracking(false);
    hide();
}

void Canvas::setSessionManager(SessionManager* session)
{
    m_session = session;
    QObject::connect(m_session, &SessionManager::sessionDeleted, this, &Canvas::onSessionDeleted);
    QObject::connect(sourceImage(), &LayerImage::imageChanged, this, &Canvas::onImageChanged);
    setFixedSize(sourceImage()->size());

    // generate background
    m_background = QPixmap(sourceImage()->size());
    QPainter painter(&m_background);
    QBrush brush;
    brush.setTexture(generateBackgroundPattern());
    painter.fillRect(m_background.rect(), brush);

    setMouseTracking(true);
    show();
}

void Canvas::onMouseInputEnabled()
{
    m_acceptMouseInput = true;
}

void Canvas::onMouseInputDisabled()
{
    m_acceptMouseInput = false;
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (m_acceptMouseInput)
    {
        if(event->button() == Qt::LeftButton && !m_isDrawing)
        {
            m_isDrawing = true;
            m_lastMousePosition = m_currentMousePosition = event->position();
            draw();
        }
    }
    event->ignore();
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (m_acceptMouseInput)
    {
        if(m_isDrawing)
        {
            m_currentMousePosition = event->position();
            if (m_lastMousePosition != m_currentMousePosition)
                draw();
            m_lastMousePosition = m_currentMousePosition;
        }
    }
    event->ignore();
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_acceptMouseInput)
    {
        if(event->button() == Qt::LeftButton && m_isDrawing)
        {
            m_currentMousePosition = event->position();
            draw();
            m_isDrawing = false;
            update();
        }
    }
    event->ignore();
}

// Generates a grid pattern using the given grid size and colors.
QPixmap Canvas::generateBackgroundPattern(const QSize &gridSize, const QColor &backgroundColor, const QColor &foregroundColor) const
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
    if (!m_session)
        return;

    if (!sourceImage()->layerAtIndex(m_session->activeLayer())->isVisible())
        return;

    int scale = pixelRatio();

    // snap mouse positions to points on widget pixel grid
    QPoint prevOnWidget = snapPointToGrid(m_lastMousePosition.toPoint());
    QPoint curOnWidget = snapPointToGrid(m_currentMousePosition.toPoint());

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
    QColor color = m_penColors[m_session->activeLayer() % m_penColors.size()];
    QPen pen(color, m_penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPainter painter;
    painter.begin(sourceImage()->layerAtIndex(m_session->activeLayer())->image());
    painter.setPen(pen);
    painter.drawLine(prevOnImage, curOnImage);
    painter.end();

    // repaint affected region of widget
    repaint(m_updateRegion);
}

void Canvas::paintEvent(QPaintEvent* event)
{
    if (!m_session)
        return;

    QPainter painter(this);

    // expand paint region to fit widget pixel grid
    QRect paintRegion = fitRectToPixel(event->rect());
    QRect imageRegion = mapRectToImage(paintRegion);

    painter.drawPixmap(paintRegion, m_background, imageRegion);
    // draw canvas layers from paint region
    for (int i = 0; i < sourceImage()->layerCount(); i++)
    {
        if (sourceImage()->layerAtIndex(i)->isVisible())
            painter.drawImage(paintRegion, *(sourceImage()->layerAtIndex(i)->image()), imageRegion);
    }
    painter.end();

    // reset update region
    m_updateRegion = QRect();
}
