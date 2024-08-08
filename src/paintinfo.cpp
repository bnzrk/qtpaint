#include "../include/paintinfo.h"

PaintInfo::PaintInfo(const QImage &original, const QImage &modified, QRect paintRegion) :
    m_paintRegion{paintRegion},
    m_original{original.copy(paintRegion)},
    m_modified{modified.copy(paintRegion)}
{
}
