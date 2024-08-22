#include "normalmapcolorselector.h"

NormalMapColorSelector::NormalMapColorSelector(QWidget *parent) :
    ColorSelector{parent},
    m_outerPadding{8},
    m_cursorOuterSize{35},
    m_cursorInnerSize{13},
    m_cursorLineLength{25}
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

void NormalMapColorSelector::updateFromColor(QColor color)
{
    m_selectedColor = color;
    update();
}

void NormalMapColorSelector::selectColor(const QPoint& pos)
{
    // Clamp position to circle
    QPoint rectPos = mapToRect(m_circleRect, pos);
    double radius = m_circleRect.width() * 0.5;

    int circleX = rectPos.x() - radius;
    int circleY = rectPos.y() - radius;
    QVector2D circlePos(circleX, circleY);
    if (circlePos.length() > radius)
    {
        circlePos.normalize();
        circlePos *= radius;
    }

    // Map to and select color
    QVector3D normal = normalAt(radius, circlePos);
    updateFromColor(normalToRGB(normal));
    emit colorSelected(m_selectedColor);
}

QColor NormalMapColorSelector::normalColorAt(double radius, int x, int y) const
{
    int circleX = x - radius;
    int circleY = y - radius;

    QVector3D normal = normalAt(radius, circleX, circleY);
    return normalToRGB(normal);
}

QColor NormalMapColorSelector::normalToRGB(const QVector3D& normal) const
{
    double r = (normal.x() + 1.0) * 0.5;
    double g = (-normal.y() + 1.0) * 0.5;
    double b = 0.5 * (normal.z() + 1.0);
    return QColor::fromRgbF(r, g, b, 1.0);
}

QVector3D NormalMapColorSelector::normalAt(double radius, const QVector2D& pos2D) const
{
    // x*x + y*y + z*z = 1
    // sqrt(1.0 - (x*x + y*y)) = z;
    double nX = pos2D.x() / radius;
    double nY = pos2D.y() / radius;
    double nZ = std::min(1.0, std::sqrt(std::max(0.0, 1.0 - (nX * nX + nY * nY))));
    return QVector3D(nX, nY, nZ);
}

QVector3D NormalMapColorSelector::normalAt(double radius, int x, int y) const
{
    // x*x + y*y + z*z = 1
    // sqrt(1.0 - (x*x + y*y)) = z;
    double nX = x / radius;
    double nY = y / radius;
    double nZ = std::min(1.0, std::sqrt(std::max(0.0, 1.0 - (nX * nX + nY * nY))));
    return QVector3D(nX, nY, nZ);
}

QVector3D NormalMapColorSelector::normalFromRGB(const QColor& color) const
{
    double nX = color.redF() * 2.0 - 1.0;
    double nY = color.greenF() * 2.0 - 1.0;
    double nZ = color.blueF() * 2.0 - 1.0;
    return QVector3D(nX, nY, nZ);
}

/*****************************************************************************
 * INPUT
 *****************************************************************************/

void NormalMapColorSelector::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        selectColor(event->pos());
    }
}

void NormalMapColorSelector::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        selectColor(event->pos());
    }
}

/*****************************************************************************
 * VIEW
 *****************************************************************************/

void NormalMapColorSelector::resizeEvent(QResizeEvent *event)
{
    updateCircleRect();
    updateCircleImage();
}

void NormalMapColorSelector::updateCircleRect()
{
    int smallestLen = (width() > height()) ? height() : width();
    int circleDiameter = smallestLen - m_outerPadding * 2;
    if (circleDiameter % 2 == 0)
        circleDiameter--;
    m_circleRect = QRect(0, 0, circleDiameter, circleDiameter);
    m_circleRect.moveCenter(rect().center());
}

void NormalMapColorSelector::showEvent(QShowEvent *event)
{
    updateFromColor(QColor(128, 128, 255));
}

void NormalMapColorSelector::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawImage(m_circleRect, m_circleImage);

    if (validNormalColor(m_selectedColor))
        drawCursor(painter);

    painter.end();
}

void NormalMapColorSelector::drawCursor(QPainter &painter)
{
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);

    // Calculate normal from selected color
    QVector3D cursorNormal = normalFromRGB(m_selectedColor);
    QPolygonF cursorOutside = projectOrthographic(centeredRect(QSize(m_cursorOuterSize, m_cursorOuterSize)), cursorNormal);
    QPolygonF cursorInside = projectOrthographic(centeredRect(QSize(m_cursorInnerSize, m_cursorInnerSize)), cursorNormal);
    QPolygonF cursorLine = projectOrthographic({QVector3D(0, 0, 0), QVector3D(0, 0, -m_cursorLineLength)}, cursorNormal);

    // Calculate cursor position from selected color
    double px = m_circleRect.width() * m_selectedColor.redF();
    double py = m_circleRect.height() * (1.0 - m_selectedColor.greenF());
    QPoint cursorPos(px, py);
    painter.translate(m_circleRect.topLeft());
    painter.translate(cursorPos.x(), cursorPos.y());

    // Paint outer polygon
    painter.setPen(QPen(Qt::white, 1.5));
    painter.setBrush(QColor(255, 255, 255, 50));
    painter.drawPolygon(cursorOutside);
    // Paint inner polygon
    painter.setBrush(m_selectedColor);
    painter.setPen(QPen(Qt::white, 1.0));
    painter.drawPolygon(cursorInside);
    // Paint line
    painter.setPen(QPen(Qt::white, 1.05));
    painter.setBrush(Qt::NoBrush);
    painter.drawPolygon(cursorLine);

    // When line angle is small paint as point for clarity
    if (inRange(cursorNormal.x(), -0.1f, 0.1f) && inRange(cursorNormal.y(), -0.1f, 0.1f))
    {
        painter.setPen(QPen(Qt::white, 1.0));
        painter.drawPoint(cursorLine.at(1));
    }
    painter.restore();
}

void NormalMapColorSelector::updateCircleImage()
{
    QImage colorMap = QImage(m_circleRect.size(), QImage::Format_ARGB32);
    QRgb *buffer = (QRgb *)colorMap.bits();
    double radius = colorMap.width() * 0.5;
    for (int y = 0; y < colorMap.height(); ++y)
    {
        for (int x = 0; x < colorMap.width(); ++x)
        {
            QColor color = normalColorAt(radius, x, y);
            buffer[y * colorMap.width() + x] = color.rgba();
        }
    }

    m_circleImage = QImage(m_circleRect.size(), QImage::Format_ARGB32);
    m_circleImage.fill(Qt::transparent);

    QPainter painter(&m_circleImage);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(colorMap));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawEllipse(0, 0, colorMap.width(), colorMap.height());
    painter.end();
}

QPolygonF NormalMapColorSelector::projectOrthographic(const QRect &rect, const QVector3D &normal) const
{
    QVector<QVector3D> points = rectToVector3D(rect);
    return projectOrthographic(points, normal);
}

QPolygonF NormalMapColorSelector::projectOrthographic(const QVector<QVector3D>& points, const QVector3D &normal) const
{
    // Construct orthographic projection matrix
    QVector3D n = QVector3D(normal.x(), -normal.y(), normal.z()).normalized();
    QVector3D up = QVector3D(0.0, 0.0, 1.0);
    QVector3D axis = QVector3D::crossProduct(n, up).normalized();
    double angle = qAcos(QVector3D::dotProduct(n, up));
    QMatrix4x4 matrix;
    matrix.ortho(-1, 1, -1, 1, -1, 1);
    matrix.rotate(qRadiansToDegrees(angle), axis);

    // Map points and add to polygon
    QVector<QPointF> transformedPoints;
    for (const QVector3D &point : points)
    {
        QVector3D point3D = matrix.mapVector(point);
        transformedPoints.append(QPointF(point3D.x(), point3D.y()));
    }

    return QPolygonF(transformedPoints);
}

QVector<QVector3D> NormalMapColorSelector::rectToVector3D(const QRect &rect) const
{
    return {QVector3D(rect.x(), rect.y(), 0),
            QVector3D(rect.x() + rect.width(), rect.y(), 0.0),
            QVector3D(rect.x() + rect.width(), rect.y() + rect.height(), 0.0),
            QVector3D(rect.x(), rect.y() + rect.height(), 0.0)};
}

bool NormalMapColorSelector::validNormalColor(const QColor& color) const
{
    return color.blue() >= 128;
}
