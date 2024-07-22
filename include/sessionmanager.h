#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include "layerimage.h"

class SessionManager : public QObject
{
    Q_OBJECT
public:
    explicit SessionManager(QObject *parent = nullptr);

    void setSourceImage(LayerImage* sourceImage);
signals:
private:
    LayerImage* m_sourceImage = nullptr;
    QVector<bool> m_selectedLayers;
    int m_activeLayer = 0;
};

#endif // SESSIONMANAGER_H
