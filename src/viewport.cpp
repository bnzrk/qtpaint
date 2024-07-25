#include "viewport.h"

Viewport::Viewport(QWidget *parent) :
    QScrollArea(parent),
    m_canvas(Canvas(this)),
    m_background(QWidget(this)),
    m_layout(QBoxLayout(QBoxLayout::LeftToRight, &m_background)),
    m_zoom(1)
{
    // setup signals and slots
    QShortcut *zoomInShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Equal), this);
    QObject::connect(zoomInShortcut, &QShortcut::activated, this, &Viewport::zoomIn);

    QShortcut *zoomOutShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus), this);
    QObject::connect(zoomOutShortcut, &QShortcut::activated, this, &Viewport::zoomOut);

    QObject::connect(this, &Viewport::childMouseInputEnabled, &m_canvas, &Canvas::onMouseInputEnabled);
    QObject::connect(this, &Viewport::childMouseInputDisabled, &m_canvas, &Canvas::onMouseInputDisabled);

    // setup widgets and layout
    setWidgetResizable(true);
    m_background.setLayout(&m_layout);
    QString style = "background-color: #404142;";
    m_background.setStyleSheet(style);
    m_layout.addWidget(&m_canvas, Qt::AlignCenter);
    setWidget(&m_background);
    setZoom(1);
}

void Viewport::keyPressEvent(QKeyEvent* event)
{
    if (m_canvas.drawing())
        return;

    if (event->key() == Qt::Key_Control && !event->isAutoRepeat())
    {
        horizontalScrollBar()->blockSignals(true);
        verticalScrollBar()->blockSignals(true);
        m_wheelZoomMode = true;
    }
    else if (event->key() == Qt::Key_Space && !event->isAutoRepeat())
    {
        emit childMouseInputDisabled();
        m_panMode = true;
        setCursor(Qt::OpenHandCursor);
    }
}

void Viewport::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control && !event->isAutoRepeat())
    {
        horizontalScrollBar()->blockSignals(false);
        verticalScrollBar()->blockSignals(false);
        m_wheelZoomMode = false;
    }
    else if (event->key() == Qt::Key_Space && !event->isAutoRepeat())
    {
        m_panMode = false;
        if (!m_panning)
        {
            emit childMouseInputEnabled();
            setCursor(Qt::ArrowCursor);
        }
    }
}

void Viewport::mousePressEvent(QMouseEvent* event)
{
    if (m_canvas.drawing())
        return;

    if (event->button() == Qt::LeftButton && m_panMode)
    {
        m_lastMousePosition = m_background.mapFromParent(event->pos());
        m_panning = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void Viewport::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_panning)
    {
        m_panning = false;
        if (m_panMode)
            setCursor(Qt::OpenHandCursor);
        else
        {
            emit childMouseInputEnabled();
            setCursor(Qt::ArrowCursor);
        }
    }
}

void Viewport::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning)
    {
        QPoint delta = m_background.mapFromParent(event->pos()) - m_lastMousePosition;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    }
}

void Viewport::wheelEvent(QWheelEvent* event)
{
    if (m_wheelZoomMode)
    {
        if (event->angleDelta().y() > 0)
            zoomIn();
        else if (event->angleDelta().y() < 0)
            zoomOut();
    }
}

void Viewport::resizeEvent(QResizeEvent* event)
{
    QScrollArea::resizeEvent(event);
    centerView();
}

void Viewport::showEvent(QShowEvent* event)
{
    QScrollArea::showEvent(event);
    m_background.setFixedSize(parentWidget()->size());
    updateView();
    centerView();
}

// Sets the current session manager.
void Viewport::setSessionManager(SessionManager* session)
{
    m_session = session;
    QObject::connect(m_session, &SessionManager::sessionDeleted, this, &Viewport::onSessionDeleted);
    m_canvas.setSessionManager(m_session);
    updateView();
    centerView();
}

// Updates display of child widgets based on source image and zoom.
void Viewport::updateView()
{
    if (m_session)
    {
        // calculate canvas size based on zoom level
        QSize canvasSize = sourceImage()->size() * m_zoom;

        // calculate and update viewport background size
        int largestLen = std::max(canvasSize.width(), canvasSize.height()) * 2;
        if (largestLen * 2 > largestLen + MAX_MARGIN_SIZE)
            m_background.setFixedSize(canvasSize.width() + MAX_MARGIN_SIZE, canvasSize.height() + MAX_MARGIN_SIZE);
        else
            m_background.setFixedSize(canvasSize.width() + largestLen, canvasSize.height() + largestLen);

        // update canvas
        m_canvas.setFixedSize(canvasSize);
        m_canvas.update();
        this->setMaximumWidth(m_background.size().width());
    }
    else
    {
        m_background.setFixedSize(size());
    }
}

// Sets the viewport's zoom level.
void Viewport::setZoom(int zoom)
{
    int prevZoom = m_zoom;
    m_zoom = zoom;

    QPoint scrollPosition = QPoint(horizontalScrollBar()->value(), verticalScrollBar()->value());
    QPoint scrollAmount = QPoint();

    // calculate new viewport position based on mouse position if canvas visible
    if (m_session)
    {
        QPoint mousePos = m_background.mapFromGlobal(QCursor::pos());
        QPoint unscaledMouseDist = mousePos / prevZoom - m_canvas.pos() / prevZoom;
        scrollAmount = unscaledMouseDist * m_zoom - unscaledMouseDist * prevZoom;
    }

    updateView();

    // update scroll bar values
    horizontalScrollBar()->setValue(scrollPosition.x() + scrollAmount.x());
    verticalScrollBar()->setValue(scrollPosition.y() + scrollAmount.y());
}

void Viewport::centerView()
{
    horizontalScrollBar()->setValue(horizontalScrollBar()->maximum() / 2);
    verticalScrollBar()->setValue(verticalScrollBar()->maximum() / 2);
}

// Zooms in the viewport by one step.
void Viewport::zoomIn()
{
    if (!m_session)
        return;

    if (m_zoom < MAX_ZOOM)
        setZoom(m_zoom + 1);
}

// Zooms out the viewport by one step.
void Viewport::zoomOut()
{
    if (!m_session)
        return;

    if (m_zoom > 1)
        setZoom(m_zoom - 1);
}
