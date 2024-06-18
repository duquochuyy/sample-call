#ifndef QTVIDEOSURFACE_H
#define QTVIDEOSURFACE_H

#include <QMainWindow>
#include <QCamera>
#include <QAbstractVideoSurface>
#include <QVideoFrame>
#include <QVideoSurfaceFormat>
#include <QDebug>
#include <QImage>
#include "./../utils/utils.h"

class QTVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    QTVideoSurface(QObject *parent = nullptr);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override;

    bool present(const QVideoFrame &frame) override;

    bool start(const QVideoSurfaceFormat &format) override;
    void stop() override;

signals:
    void frameCaptured(const std::shared_ptr<uchar> yuv420pData, int width, int height, uint64_t timestamp);

private:
    void processYUVDataToGrayScale(const uchar *yuvData, int width, int height);
    std::shared_ptr<uchar> processNV12DatatToYUV420(const uchar *nv12Data, int width, int height);
};

#endif // QTVIDEOSURFACE_H
