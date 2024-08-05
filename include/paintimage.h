#ifndef PAINTIMAGE_H
#define PAINTIMAGE_H

#include <QObject>
#include <QImage>

/*
 * Manages an image that can be modified non-destructively when the buffer is active.
 *
 * NOTE: Finalizing the buffer overwrites the image with the buffer and returns the original
 * image before changes were made. The buffer can otherwise be disabled by discarding it.
 */
class PaintImage : public QObject
{
    Q_OBJECT
public:
    explicit PaintImage(QImage image, QObject *parent = nullptr);
    ~PaintImage();
    QImage* image();

    bool bufferActive() const;
public slots:
    void enableBuffer();
    QImage finalizeBuffer();
    void discardBuffer();
protected:
    QImage* m_buffer;
    QImage m_image;

    bool m_bufferActive;
};

#endif // PAINTIMAGE_H
