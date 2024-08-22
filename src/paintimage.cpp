#include "../include/paintimage.h"

PaintImage::PaintImage(QImage image, QObject *parent) :
    QObject{parent},
    m_image{image},
    m_buffer{nullptr},
    m_bufferActive{false}
{}

PaintImage::~PaintImage()
{
    delete m_buffer;
}

void PaintImage::enableBuffer()
{
    m_buffer = new QImage(m_image);
    m_bufferActive = true;
}

QImage PaintImage::finalizeBuffer()
{
    QImage original = m_image;

    m_image = *m_buffer;
    delete m_buffer;
    m_buffer = nullptr;
    m_bufferActive = false;

    return original;
}

void PaintImage::discardBuffer()
{
    delete m_buffer;
    m_buffer = nullptr;
    m_bufferActive = false;
}

QImage* PaintImage::image()
{
    if (m_bufferActive)
        return m_buffer;
    else
        return &m_image;
}
