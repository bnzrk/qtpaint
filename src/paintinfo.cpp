#include "../include/paintinfo.h"

PaintInfo::PaintInfo(const QImage &original, const QImage &modified, QRect paintRegion) :
    m_paintRegion{paintRegion},
    m_original{QImage(paintRegion.size(), QImage::Format_ARGB32)},
    m_modified{QImage(paintRegion.size(), QImage::Format_ARGB32)}
{
    // Initialize image regions
    QPainter painter(&m_original);
    painter.drawImage(m_original.rect(), original, m_paintRegion);
    painter.end();
    painter.begin(&m_modified);
    painter.drawImage(m_modified.rect(), modified, m_paintRegion);
}
