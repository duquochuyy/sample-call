#include "./QtVideoRender.h"

#include <QPixmap>

QtVideoRender::QtVideoRender() : frameCount(0) {
    startTime = std::chrono::steady_clock::now();
}

QtVideoRender::~QtVideoRender() {}

void QtVideoRender::setVideoFrameLabel(std::shared_ptr<YuvWidget> &widget) {
    _localRenderWidget = widget;
}

void QtVideoRender::render(const std::shared_ptr<void> &frame,
                           ImageFormat format) {
    if (format == ImageFormat::NV12) {
        auto framePtr = std::static_pointer_cast<ZRootFrame>(frame);
        _localRenderWidget->setFrameData(framePtr->nv12Data, framePtr->width,
                                         framePtr->height);
        getInfo(framePtr->width, framePtr->height);
    } else {
        qDebug() << "Unsupported image format";
        return;
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
