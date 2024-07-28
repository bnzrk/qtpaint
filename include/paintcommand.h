#ifndef PAINTCOMMAND_H
#define PAINTCOMMAND_H

#include "command.h"
#include "canvas.h"

/*
 * Abstract class for commands that perform some operation on a canvas's image.
 */
class PaintCommand : public Command
{
public:
    explicit PaintCommand(Canvas* canvas, QVector<int> targetLayers) : m_canvas{canvas}, m_targetLayers{targetLayers} {}
    virtual QRect affectedRegion() const = 0;
protected:
    Canvas* m_canvas;
    QVector<int> m_targetLayers;
};

#endif // PAINTCOMMAND_H
