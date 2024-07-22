#include "../include/sessionmanager.h"

SessionManager::SessionManager(QObject *parent)
    : QObject{parent}
{}

void SessionManager::setSourceImage(LayerImage* sourceImage)
{
    m_sourceImage = sourceImage;
}
