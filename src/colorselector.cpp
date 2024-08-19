#include "colorselector.h"

ColorSelector::ColorSelector(QWidget *parent) : QWidget{parent}
{}

QRect ColorSelector::centeredRect(const QSize& size)
{
    QRect rect(QPoint(0, 0), size);
    rect.moveCenter(rect.topLeft());
    return rect;
}

int ColorSelector::integerRadius(int diameter)
{
    if (diameter % 2 == 0)
        return (diameter - 1) / 2;
    else
        return diameter / 2;
}

QPoint ColorSelector::mapToRect(const QRect& rect, const QPoint& point)
{
    return QPoint(point.x() - rect.x(), point.y() - rect.y());
}

QPoint ColorSelector::mapToCircle(double radius, const QPoint &point)
{
    return QPoint(point.x() - radius, point.y() - radius);
}

bool ColorSelector::circleContains(int radius, const QPoint& point)
{
    int x = point.x() - radius;
    int y = point.y() - radius;

    int distSqrd = x * x + y * y;
    return !(distSqrd > radius * radius);
}

bool ColorSelector::circleContains(double radius, const QPoint& point)
{
    int x = point.x() - radius;
    int y = point.y() - radius;

    int distSqrd = x * x + y * y;
    return !(distSqrd > radius * radius);
}

bool ColorSelector::ringContains(const QRect& outerCircle, const QRect& innerCircle, const QPoint& point)
{
    double radius = outerCircle.width() * 0.5;

    double dx = point.x() - outerCircle.center().x();
    double dy = point.y() - outerCircle.center().y();

    double distSquared = dx * dx + dy * dy;
    if (distSquared > radius * radius)
        return false;

    radius = innerCircle.width() * 0.5;

    dx = point.x() - innerCircle.center().x();
    dy = point.y() - innerCircle.center().y();
    distSquared = dx * dx + dy * dy;

    return (distSquared > radius * radius);
}
