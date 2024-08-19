#ifndef COLORSELECTOR_H
#define COLORSELECTOR_H

#include <QWidget>
#include <QPainter>
#include <QVector3D>
#include <QMouseEvent>

class ColorSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ColorSelector(QWidget *parent = nullptr);

    virtual void selectColor(const QPoint& pos) = 0;
public slots:
    virtual void updateFromColor(QColor color) = 0;
signals:
    void colorSelected(QColor color);
public:
    static QPoint mapToRect(const QRect& rect, const QPoint& point);
    static QPoint mapToCircle(double radius, const QPoint& point);
    static QRect centeredRect(const QSize& size);

    static bool circleContains(int radius, const QPoint& point);
    static bool circleContains(double radius, const QPoint& point);
    static bool ringContains(const QRect& outerDiameter, const QRect& innerDiameter, const QPoint& point);

    template <typename T>
    static bool inRange(const T& value, const T& min, const T& max) { return !(value < min || value > max); }
    static int integerRadius(int diameter);
protected:
    const QColor BG_COLOR = QColor(50, 50, 50, 255);
};

#endif // COLORSELECTOR_H
