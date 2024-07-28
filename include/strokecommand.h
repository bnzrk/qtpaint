#ifndef STROKECOMMAND_H
#define STROKECOMMAND_H

#include <QImage>
#include <QRect>
#include <QPainter>
#include "paintcommand.h"
#include "stroke.h"

/*
 * Command that paints a stroke on a canvas layer.
 */
class StrokeCommand : public PaintCommand
{
public:
    explicit StrokeCommand(Canvas* canvas, int targetLayer, Stroke stroke);
    virtual void execute() const override;
    virtual void revert() const override;
    virtual QRect affectedRegion() const override { return m_stroke.boundingRect(); }
private:
    int targetLayer() const { return m_targetLayers[0]; }
private:
    Stroke m_stroke;
    QImage m_buffer;
};

#endif // STROKECOMMAND_H
