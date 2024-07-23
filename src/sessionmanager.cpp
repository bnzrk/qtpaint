#include "../include/sessionmanager.h"

SessionManager::SessionManager(QObject *parent)
    : QObject{parent}
{
    LayerImage* sourceImage = new LayerImage(800, 600, this);
    setSourceImage(sourceImage);
}


void SessionManager::setSourceImage(LayerImage* sourceImage)
{
    m_sourceImage = sourceImage;
}
