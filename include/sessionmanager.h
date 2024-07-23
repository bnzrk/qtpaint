#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include "layerimage.h"

class SessionManager : public QObject
{
    Q_OBJECT
public:
    explicit SessionManager(QObject *parent = nullptr);
    ~SessionManager() { emit sessionDeleted(); }

    void setSourceImage(LayerImage* sourceImage);
    LayerImage* sourceImage() { return m_sourceImage; }

    int activeLayer() const { return m_activeLayer; }
    void setActiveLayer(int index) { m_activeLayer = index; }

    QVector<int> selectedLayers() const { return m_selectedLayers.values(); }
    bool inSelectedLayers(int index) const { return m_selectedLayers.contains(index); }
    int selectedLayerCount() const { return m_selectedLayers.size(); }
    void clearSelectedLayers() { m_selectedLayers.clear(); }
    void addSelectedLayer(int index) { m_selectedLayers.insert(index); }
    void removeSelectedLayer(int index) { m_selectedLayers.remove(index); }
signals:
    void sessionDeleted();
private:
    LayerImage* m_sourceImage = nullptr;
    QSet<int> m_selectedLayers;
    int m_activeLayer = 0;
};

#endif // SESSIONMANAGER_H
