#include "hsvcolorselector.h"

HSVColorSelector::HSVColorSelector(QWidget *parent) :
    ColorSelector{parent},
    m_outerPadding{8},
    m_boxCursorWidth{11},
    m_ringWidthRatio{0.85},
    m_ringCursorExtension{2},
    m_ringCursorMaxHeight{9},
    m_ringCursorMinHeight{5},
    m_ringCursorHeightRatio{0.6},
    m_draggingRing{false},
    m_draggingBox{false}
{
    QSizePolicy sp;
    sp.setHorizontalStretch(1);
    sp.setVerticalStretch(1);
    sp.setHorizontalPolicy(QSizePolicy::Expanding);
    sp.setVerticalPolicy(QSizePolicy::Expanding);
    setSizePolicy(sp);

    setMinimumSize(61, 61);
    resize(249, 249);
}

/*****************************************************************************
 * CONTROL
 *****************************************************************************/

void HSVColorSelector::updateFromColor(QColor color)
{
    bool hueChanged = color.hue() != m_selectedColor.hue();
    m_selectedColor = color;
    if (hueChanged)
        updateBoxImage();
}

void HSVColorSelector::selectHue(const QPoint& pos)
{
    m_selectedColor.setHsv(getHue(pos), m_selectedColor.saturation(), m_selectedColor.value());
    updateBoxImage();
    emit colorSelected(m_selectedColor);
    update();
}

void HSVColorSelector::selectSatVal(const QPoint& pos)
{
    m_selectedColor = getHSV(m_boxRect.size(), pos);
    emit colorSelected(m_selectedColor);
    update();
}

void HSVColorSelector::selectColor(const QPoint& pos)
{
    if (!m_draggingRing && m_draggingBox)
    {
        selectSatVal(mapToRect(m_boxRect, pos));
    }
    else if (!m_draggingBox && m_draggingRing)
    {
        double radius = m_ringOuterRect.width() * 0.5;
        selectHue(mapToCircle(radius, mapToRect(m_ringOuterRect, pos)));
    }
}

/*****************************************************************************
 * INPUT
 *****************************************************************************/

void HSVColorSelector::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (ringContains(m_ringOuterRect, m_ringInnerRect, event->pos()))
        {
            m_draggingRing = true;
            selectColor(event->pos());
        }
        else if (m_boxRect.contains(event->pos()))
        {
            m_draggingBox = true;
            selectColor(event->pos());
        }
    }
}

void HSVColorSelector::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        selectColor(event->pos());
    }
}

void HSVColorSelector::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_draggingRing = m_draggingBox = false;
    }
}

/*****************************************************************************
 * VIEW
 *****************************************************************************/

void HSVColorSelector::showEvent(QShowEvent *event)
{
    updateRingImage();
    updateFromColor(QColor::fromHsv(getHue(0), 255, 255, 255));
}

void HSVColorSelector::resizeEvent(QResizeEvent *event)
{
    // Recalculate visuals
    int smallestLen = width();
    if (width() > height())
        smallestLen = height();

    int ringOuterDiameter = smallestLen - m_outerPadding * 2;
    if (ringOuterDiameter % 2 == 0)
        ringOuterDiameter--;
    m_ringOuterRect = QRect(0, 0, ringOuterDiameter, ringOuterDiameter);
    m_ringOuterRect.moveCenter(rect().center());

    int ringInnerDiameter = ringOuterDiameter * m_ringWidthRatio;
    if (ringInnerDiameter % 2 == 0)
    {
        ringInnerDiameter--;
        // Ensure odd width ring
        if ((ringOuterDiameter - ringInnerDiameter) % 2 == 0)
            ringInnerDiameter += 2;
    }
    m_ringInnerRect = QRect(0, 0, ringInnerDiameter, ringInnerDiameter);
    m_ringInnerRect.moveCenter(rect().center());

    int boxLength = integerRadius(ringInnerDiameter) * std::sqrt(2) * 0.90;
    m_boxRect = QRect(0, 0, boxLength, boxLength);
    m_boxRect.moveCenter(rect().center());

    updateRingImage();
    updateBoxImage();
}

void HSVColorSelector::paintEvent(QPaintEvent* event)
{
    // FIXME: Cursors can overlap
    // FIXME: Qt painter/transform rotations not accurate enough around a center point
    QPainter painter(this);
    painter.drawImage(m_ringOuterRect, m_ringImage);
    painter.drawImage(m_boxRect, m_boxImage);
    drawRingCursor(painter);
    drawBoxCursor(painter);
    painter.end();
}

void HSVColorSelector::drawRingCursor(QPainter &painter)
{
    painter.save();
    double outerEdgeRadius = m_ringOuterRect.width() * 0.5;
    double innerEdgeRadius = m_ringInnerRect.width() * 0.5 + 1;
    double centerRadius = (outerEdgeRadius + innerEdgeRadius) * 0.5;
    int ringWidth = ((m_ringOuterRect.width() * 0.5) - (m_ringInnerRect.width() * 0.5));

    int cursorWidth = ringWidth + m_ringCursorExtension * 2;
    int cursorHeight = std::clamp(static_cast<int>(cursorWidth * m_ringCursorHeightRatio), m_ringCursorMinHeight, m_ringCursorMaxHeight);
    QRect cursorRect = QRect(0, 0, cursorWidth, cursorHeight);
    cursorRect.moveCenter(cursorRect.topLeft());
    cursorRect.translate(0, -1);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(m_ringOuterRect.center());
    painter.rotate(m_selectedColor.hue());
    painter.translate(QPoint(-centerRadius, 0));
    painter.setPen(Qt::NoPen);
    painter.setBrush(BG_COLOR);
    painter.drawRoundedRect(cursorRect, 3.0, 3.0);
    cursorRect.adjust(1, 1, -1, -1);
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(cursorRect, 2.0, 2.0);
    painter.setBrush(QColor::fromHsv(m_selectedColor.hue(), 255, 255));
    painter.drawRoundedRect(cursorRect.toRectF().adjusted(1.25, 1.25, -1.25, -1.25), 1, 1);
    painter.restore();
}

void HSVColorSelector::drawBoxCursor(QPainter &painter)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QRect cursorRect = QRect(0, 0, m_boxCursorWidth, m_boxCursorWidth);
    cursorRect.translate(-cursorRect.center());
    painter.setPen(Qt::NoPen);
    painter.setBrush(BG_COLOR);
    painter.translate(m_boxRect.left() + m_selectedColor.saturationF() * m_boxRect.width(), m_boxRect.top() + (1.0 - m_selectedColor.valueF()) * m_boxRect.height());
    painter.drawEllipse(cursorRect);
    painter.setBrush(Qt::white);
    painter.drawEllipse(cursorRect.adjusted(1, 1, -1, -1));
    painter.setBrush(m_selectedColor);
    QRectF color = cursorRect.toRectF();
    painter.drawEllipse(color.adjusted(2.25, 2.25, -2.25, -2.25));
    painter.restore();
}

void HSVColorSelector::updateRingImage()
{
    QImage colorMap = QImage(m_ringOuterRect.size(), QImage::Format_ARGB32);
    QRgb *buffer = (QRgb *)colorMap.bits();
    for (int y = 0; y < colorMap.height(); ++y)
    {
        for (int x = 0; x < colorMap.width(); ++x)
        {
            double radius = colorMap.width() * 0.5;
            QPoint point = QPoint(x - radius, y - radius);
            QColor color = QColor::fromHsv(getHue(point), 255, 255, 255);
            buffer[y * colorMap.width() + x] = color.rgba();
        }
    }

    m_ringImage = QImage(m_ringOuterRect.size(), QImage::Format_ARGB32);
    m_ringImage.fill(Qt::transparent);

    QPainter painter(&m_ringImage);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(colorMap));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawEllipse(0, 0, colorMap.width(), colorMap.height());

    painter.setBrush(Qt::transparent);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.drawEllipse((m_ringOuterRect.width() - m_ringInnerRect.width()) * 0.5, (m_ringOuterRect.height() - m_ringInnerRect.height()) * 0.5, m_ringInnerRect.width(), m_ringInnerRect.height());
    painter.end();
}

void HSVColorSelector::updateBoxImage()
{
    QImage colorMap = QImage(m_boxRect.size(), QImage::Format_ARGB32);
    QRgb *buffer = (QRgb *)colorMap.bits();
    for (int y = 0; y < colorMap.height(); ++y)
    {
        for (int x = 0; x < colorMap.width(); ++x)
        {
            QColor color = getHSV(colorMap.size(), QPoint(x, y));
            buffer[y * colorMap.width() + x] = color.rgba();
        }
    }

    m_boxImage = colorMap;
}

int HSVColorSelector::getHue(double angleRadians) const
{
    double angle = angleRadians / (2 * M_PI);

    return angle * 359;
}

int HSVColorSelector::getHue(const QPoint& point) const
{
    double angle = std::atan2(point.y(), point.x());
    angle += M_PI;

    return getHue(angle);
}

QColor HSVColorSelector::getHSV(const QSize& size, const QPoint& point) const
{
    double dx = static_cast<double>(point.x()) / (size.width() - 1);
    double dy = 1.0 - static_cast<double>(point.y()) / (size.height() - 1);

    // Set hue from angle
    QColor color;
    color.setAlpha(255);
    color.setHsvF(m_selectedColor.hueF(), std::clamp(dx, 0.0, 1.0), std::clamp(dy, 0.0, 1.0));

    return color;
}
