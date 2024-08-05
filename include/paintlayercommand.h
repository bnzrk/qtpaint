#ifndef PAINTLAYERCOMMAND_H
#define PAINTLAYERCOMMAND_H

#include "paintcommand.h"
#include "paintinfo.h"

/*
 * Command that paints a region of the specified layer.
 */
class PaintLayerCommand : public PaintCommand
{
public:
    PaintLayerCommand(Canvas* canvas, int targetLayer, PaintInfo paintInfo);

    virtual QRect affectedRegion() const override { return m_paintInfo.paintRegion(); }
    virtual void execute() const override;
    virtual void revert() const override;
private:
    int targetLayer() const { return m_targetLayers[0]; }
private:
    PaintInfo m_paintInfo;
};

#endif // PAINTLAYERCOMMAND_H
