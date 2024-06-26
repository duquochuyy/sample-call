#include "./PartnerVideoRender.h"

#include <QPixmap>

#include "./../Utils/Define.h"

PartnerVideoRender::PartnerVideoRender() : frameCount(0) {
    startTime = std::chrono::steady_clock::now();
    image = QImage(WIDTH, HEIGHT, QImage::Format_RGB32);
}

PartnerVideoRender::~PartnerVideoRender() { delete _label; }

void PartnerVideoRender::setVideoFrameLabel(QLabel *&label) { _label = label; }

void PartnerVideoRender::render(const std::shared_ptr<void> &frame,
                                ImageFormat format) {
    if (format == ImageFormat::YUV420) {
        auto framePtr = std::static_pointer_cast<ZVideoFrame>(frame);
        _convert->convertYUV420ToRGB(framePtr.get()->yuv420pData,
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
};

void PartnerVideoRender::getInfo(int width, int height) {
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0) {
        int fps = frameCount.load() / elapsedSeconds.count();
        frameCount = 0;
        startTime = currentTime;

        if (_callback != nullptr)
            _callback->onShowInfoPartnerFps(fps);
    }
}