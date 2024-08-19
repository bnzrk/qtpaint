#ifndef HSVCOLORSELECTOR_H
#define HSVCOLORSELECTOR_H

#include "colorselector.h"

class HSVColorSelector : public ColorSelector
{
    Q_OBJECT
public:
    explicit HSVColorSelector(QWidget *parent = nullptr);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void updateRingImage();
    void updateBoxImage();

    void drawRingCursor(QPainter& painter);
    void drawBoxCursor(QPainter& painter);

    void selectHue(const QPoint& pos);
    void selectSatVal(const QPoint& pos);
    void selectColor(const QPoint& pos) override;

    int getHue(double angleRadians) const;
    int getHue(const QPoint& point) const;
    QColor getHSV(const QSize& size, const QPoint& point) const;
public slots:
    virtual void updateFromColor(QColor color) override;
private:
    QColor m_selectedColor;

    int m_outerPadding;
    double m_ringWidthRatio;
    int m_boxCursorWidth;
    int m_ringCursorExtension;
    int m_ringCursorMaxHeight;
    int m_ringCursorMinHeight;
    double m_ringCursorHeightRatio;

    QImage m_ringImage;
    QRect m_ringOuterRect;
    QRect m_ringInnerRect;

    QImage m_boxImage;
    QRect m_boxRect;

    bool m_draggingRing;
    bool m_draggingBox;
};

#endif // HSVCOLORSELECTOR_H
