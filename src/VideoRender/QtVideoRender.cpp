#include "./QtVideoRender.h"

#include <QPixmap>

QtVideoRender::QtVideoRender() : frameCount(0) {
    startTime = std::chrono::steady_clock::now();
    image = QImage(WIDTH, HEIGHT, QImage::Format_RGB32);
}

QtVideoRender::~QtVideoRender() { delete _label; }

void QtVideoRender::setVideoFrameLabel(QLabel *&label) { _label = label; }

void QtVideoRender::render(const std::shared_ptr<void> &frame,
                           ImageFormat format) {
    if (format == ImageFormat::NV12) {
        auto framePtr = std::static_pointer_cast<ZRootFrame>(frame);
        _convert->processNV12DatatToRGB(framePtr.get()->nv12Data,
                                        framePtr.get()->width,
                                        framePtr.get()->height, image);
    } else {
        qDebug() << "Unsupported image format";
        return;
    }
    if (image.isNull())  // Kiểm tra xem QImage có trống không
    {
        qDebug() << "Empty image received.";
        return;
    }
    if (_label != nullptr) {
        _label->setPixmap(QPixmap::fromImage(image).scaled(
            _label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        getInfo(image.width(), image.height());
    }
}

void QtVideoRender::getInfo(int width, int height) {
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0) {
        int fps = frameCount.load() / elapsedSeconds.count();
        frameCount = 0;
        startTime = currentTime;

        if (_callback != nullptr)
            _callback->onShowInfoLocalFps(fps);
    }
}
