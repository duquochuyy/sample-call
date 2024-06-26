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
#include "./../Struct/ZRootFrame.h"

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

private:
    void getInfo();
signals:
    void frameCapturedRawFormat(const ZRootFrame &frame);

private:
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    std::atomic<int> frameCount;
};

#endif // QTVIDEOSURFACE_H
