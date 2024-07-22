#ifndef LAYER_H
#define LAYER_H

#include <QImage>
#include <QObject>

class Layer : public QObject
{
    Q_OBJECT
public:
    explicit Layer(QString name, QImage image, QObject* parent = nullptr);

    QString getName() const { return m_name; }
    QImage* getImage() { return &m_image; }

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
