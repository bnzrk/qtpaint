#ifndef LAYER_H
#define LAYER_H

#include <QImage>
#include <QObject>

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(QString name, QImage image, QObject* parent = nullptr);

    QString name() const { return m_name; }
    QImage* image() { return &m_image; }

    void setPixel(int x, int y, QRgb color) { m_image.setPixel(QPoint(x, y), color); }
    void fill(QRgb fillColor);

    bool isVisible() const { return m_visible; }
    void setVisible(bool visible);
    void toggleVisible();
signals:
    void layerChanged();
private:
    QString m_name;
    QImage m_image;

    bool m_visible = true;
};

#endif // LAYER_H
