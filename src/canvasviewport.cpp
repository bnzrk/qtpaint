#include "canvasviewport.h"

CanvasViewport::CanvasViewport(QWidget *parent) :
    QScrollArea(parent),
    m_zoom(1)
{
    m_background = new QWidget(this);
    m_layout = new QBoxLayout(QBoxLayout::LeftToRight, m_background);
    m_canvasWidget = new CanvasWidget(this);

    // setup signals and slots
    QShortcut *zoomInShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Equal), this);
    QObject::connect(zoomInShortcut, &QShortcut::activated, this, &CanvasViewport::zoomIn);

    QShortcut *zoomOutShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus), this);
    QObject::connect(zoomOutShortcut, &QShortcut::activated, this, &CanvasViewport::zoomOut);

    // TEMP
    QShortcut *undoShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z), this);
    QObject::connect(undoShortcut, &QShortcut::activated, this, [=]{ m_canvas->undo(); m_canvasWidget->update(); });

    QShortcut *redoShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Y), this);
    QObject::connect(redoShortcut, &QShortcut::activated, this, [=]{ m_canvas->redo(); m_canvasWidget->update(); });

    QObject::connect(this, &CanvasViewport::childMouseInputEnabled, m_canvasWidget, &CanvasWidget::onMouseInputEnabled);
    QObject::connect(this, &CanvasViewport::childMouseInputDisabled, m_canvasWidget, &CanvasWidget::onMouseInputDisabled);

    // setup widgets and layout
    setWidgetResizable(true);
    m_background->setLayout(m_layout);
    m_background->setObjectName("viewportBackground");
    m_layout->addWidget(m_canvasWidget, Qt::AlignCenter);
    setWidget(m_background);
}

CanvasViewport::~CanvasViewport()
{
}

void CanvasViewport::setCanvas(Canvas* canvas)
{
    m_canvas = canvas;
    m_canvasWidget->setCanvas(m_canvas);

    if (!m_canvas)
    {
        hide();
        return;
    }

    setZoom(1);
    updateView();
    centerView();
}

void CanvasViewport::keyPressEvent(QKeyEvent* event)
{
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

void CanvasViewport::keyReleaseEvent(QKeyEvent* event)
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

void CanvasViewport::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_panMode)
    {
        m_lastMousePosition = m_background->mapFromParent(event->pos());
        m_panning = true;
        setCursor(Qt::ClosedHandCursor);
    }
}

void CanvasViewport::mouseReleaseEvent(QMouseEvent* event)
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

void CanvasViewport::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning)
    {
        QPoint delta = m_background->mapFromParent(event->pos()) - m_lastMousePosition;
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
    }
}

void CanvasViewport::wheelEvent(QWheelEvent* event)
{
    if (m_wheelZoomMode)
    {
        if (event->angleDelta().y() > 0)
            zoomIn();
        else if (event->angleDelta().y() < 0)
            zoomOut();
    }
}

void CanvasViewport::resizeEvent(QResizeEvent* event)
{
    QScrollArea::resizeEvent(event);
    centerView();
}

void CanvasViewport::showEvent(QShowEvent* event)
{
    QScrollArea::showEvent(event);
    m_background->setFixedSize(parentWidget()->size());
    updateView();
    centerView();
}

// Updates display of child widgets based on source image and zoom.
void CanvasViewport::updateView()
{
    if (m_canvas)
    {
        // calculate canvas size based on zoom level
        QSize canvasSize = m_canvas->size() * m_zoom;

        // calculate and update CanvasViewport background size
        int largestLen = std::max(canvasSize.width(), canvasSize.height()) * 2;
        if (largestLen * 2 > largestLen + MAX_MARGIN_SIZE)
            m_background->setFixedSize(canvasSize.width() + MAX_MARGIN_SIZE, canvasSize.height() + MAX_MARGIN_SIZE);
        else
            m_background->setFixedSize(canvasSize.width() + largestLen, canvasSize.height() + largestLen);

        // update canvas
        m_canvasWidget->setFixedSize(canvasSize);
        m_canvasWidget->update();

        // FIXME: Rewrite resizing of viewport background
        //this->setMaximumWidth(m_background->size().width());
    }
    else
    {
        m_background->setFixedSize(size());
    }
}

// Sets the CanvasViewport's zoom level.
void CanvasViewport::setZoom(int zoom)
{
    int prevZoom = m_zoom;
    m_zoom = zoom;

    m_canvas->setZoom(zoom);

    QPoint scrollPosition = QPoint(horizontalScrollBar()->value(), verticalScrollBar()->value());
    QPoint scrollAmount = QPoint();

    // calculate new CanvasViewport position based on mouse position if canvas visible
    if (m_canvas)
    {
        QPoint mousePos = m_background->mapFromGlobal(QCursor::pos());
        QPoint unscaledMouseDist = mousePos / prevZoom - m_canvasWidget->pos() / prevZoom;
        scrollAmount = unscaledMouseDist * m_zoom - unscaledMouseDist * prevZoom;
    }

    updateView();

    // update scroll bar values
    horizontalScrollBar()->setValue(scrollPosition.x() + scrollAmount.x());
    verticalScrollBar()->setValue(scrollPosition.y() + scrollAmount.y());
}

void CanvasViewport::centerView()
{
    horizontalScrollBar()->setValue(horizontalScrollBar()->maximum() / 2);
    verticalScrollBar()->setValue(verticalScrollBar()->maximum() / 2);
}

// Zooms in the CanvasViewport by one step.
void CanvasViewport::zoomIn()
{
    if (!m_canvas)
        return;

    if (m_canvas->zoom() < MAX_ZOOM)
        setZoom(m_zoom + 1);
}

// Zooms out the CanvasViewport by one step.
void CanvasViewport::zoomOut()
{
    if (!m_canvas)
        return;

    if (m_canvas->zoom() > 1)
        setZoom(m_zoom - 1);
}
