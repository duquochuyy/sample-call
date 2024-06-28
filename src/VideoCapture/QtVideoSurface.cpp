#include "./QtVideoSurface.h"

QTVideoSurface::QTVideoSurface(QObject *parent)
    : QAbstractVideoSurface(parent), frameCount(0) {}

QList<QVideoFrame::PixelFormat> QTVideoSurface::supportedPixelFormats(
    QAbstractVideoBuffer::HandleType handleType) const {
    Q_UNUSED(handleType);
    return {QVideoFrame::Format_YUV420P, QVideoFrame::Format_YV12,
            QVideoFrame::Format_NV12,    QVideoFrame::Format_NV21,
            QVideoFrame::Format_YUV422P, QVideoFrame::Format_YUYV,
            QVideoFrame::Format_YUV444};
}

bool QTVideoSurface::present(const QVideoFrame &frame) {
    if (frame.isValid()) {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);

        if (cloneFrame.pixelFormat() == QVideoFrame::Format_NV12) {
            getInfo();
            const uchar *nv12Data = cloneFrame.bits();
            int width = cloneFrame.width();
            int height = cloneFrame.height();
            const uint64_t timestamp = Utils::getCurrentTimestamp();
            ZRootFrame frame(nv12Data, width * height * 3 / 2, width, height,
                             timestamp);
            emit frameCapturedRawFormat(frame);
        }

        cloneFrame.unmap();
        return true;
    }
    return false;
}

bool QTVideoSurface::start(const QVideoSurfaceFormat &format) {
    qDebug() << "Starting video surface with format:" << format.pixelFormat();
    return QAbstractVideoSurface::start(format);
}

void QTVideoSurface::stop() {
    qDebug() << "Stopping video surface";
    QAbstractVideoSurface::stop();
}

void QTVideoSurface::getInfo() {
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0) {
        int fps = frameCount.load() / elapsedSeconds.count();
        frameCount = 0;
        startTime = currentTime;
    }
}