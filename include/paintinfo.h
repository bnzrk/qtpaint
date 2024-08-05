#ifndef PAINTINFO_H
#define PAINTINFO_H

#include <QImage>
#include <QPainter>

/*
 * Holds information about a paint operation.
 */
class PaintInfo
{
public:
    PaintInfo(const QImage& original, const QImage& modified, QRect paintRegion);

    QRect paintRegion() const { return m_paintRegion; }
    QImage original() const { return m_original; }
    QImage modified() const { return m_modified; }
private:
    QRect m_paintRegion;
    QImage m_original;
    QImage m_modified;
};

#endif // PAINTINFO_H
