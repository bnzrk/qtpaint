#ifndef NORMALMAPCOLORSELECTOR_H
#define NORMALMAPCOLORSELECTOR_H

#include <QtMath>
#include <QMatrix4x4>
#include "colorselector.h"

class NormalMapColorSelector : public ColorSelector
{
    Q_OBJECT
public:
    explicit NormalMapColorSelector(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void drawCursor(QPainter& painter);

    void updateCircleImage();
    void updateCircleRect();

    void selectColor(const QPoint& pos) override;

    QColor normalColorAt(double radius, int x, int y) const;
    QVector3D normalAt(double radius, const QVector2D& vec2) const;
    QVector3D normalAt(double radius, int x, int y) const;
    QColor normalToRGB(const QVector3D& normal) const;
    QVector3D normalFromRGB(const QColor& color) const;

    bool validNormalColor(const QColor& color) const;

    QVector<QVector3D> rectToVector3D(const QRect &rect) const;
    QPolygonF projectOrthographic(const QRect& rect, const QVector3D& normal) const;
    QPolygonF projectOrthographic(const QVector<QVector3D>& points, const QVector3D& normal) const;
public slots:
    virtual void updateFromColor(QColor color) override;
private:
    QColor m_selectedColor;
    int m_outerPadding;

    QImage m_circleImage;
    QRect m_circleRect;

    int m_cursorOuterSize;
    int m_cursorInnerSize;
    int m_cursorLineLength;
};

#endif // NORMALMAPCOLORSELECTOR_H
