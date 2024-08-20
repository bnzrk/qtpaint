#ifndef LAYER_H
#define LAYER_H

#include <QImage>
#include <QPainter>
#include "paintimage.h"

class Layer : public PaintImage
{
    Q_OBJECT
public:
    enum class BlendMode
    {
        Normal = QPainter::CompositionMode::CompositionMode_SourceOver,
        Multiply = QPainter::CompositionMode::CompositionMode_Multiply,
        Add = QPainter::CompositionMode::CompositionMode_Plus,
        ColorBurn = QPainter::CompositionMode::CompositionMode_ColorBurn,
        ColorDodge = QPainter::CompositionMode::CompositionMode_ColorDodge,
        Screen = QPainter::CompositionMode::CompositionMode_Screen,
        Overlay = QPainter::CompositionMode::CompositionMode_Overlay,
        Lighten = QPainter::CompositionMode::CompositionMode_Lighten,
        Darken = QPainter::CompositionMode::CompositionMode_Darken,
        HardLight = QPainter::CompositionMode::CompositionMode_HardLight,
        SoftLight = QPainter::CompositionMode::CompositionMode_SoftLight,
        Difference = QPainter::CompositionMode::CompositionMode_Difference,
        Exclusion = QPainter::CompositionMode::CompositionMode_Exclusion,
        Xor = QPainter::CompositionMode::CompositionMode_Xor,
    };
public:
    explicit Layer(QString name, QImage image, QObject* parent = nullptr);

    QString name() const { return m_name; }

    void fill(QRgb fillColor);
    void clear();

    BlendMode blendMode() { return m_blendMode; }
    void setBlendMode(BlendMode blendMode);
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

    BlendMode m_blendMode;
    bool m_visible = true;
};

#endif // LAYER_H
