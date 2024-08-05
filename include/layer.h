#ifndef LAYER_H
#define LAYER_H

#include <QImage>
#include "paintimage.h"

class Layer : public PaintImage
{
    Q_OBJECT
public:
    explicit Layer(QString name, QImage image, QObject* parent = nullptr);

    QString name() const { return m_name; }

    void fill(QRgb fillColor);
    void clear();

    bool isVisible() const { return m_visible; }
    void setVisible(bool visible);
    void toggleVisible();
public slots:
    void markDirty(const QRect& dirtyRegion);
signals:
    void layerChanged();
    void layerImageChanged(const QRect& affectedRegion);
private:
    QString m_name;

    bool m_visible = true;
};

#endif // LAYER_H
