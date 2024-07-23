#include "./PartnerVideoRender.h"

#include <QPixmap>

#include "./../Common/ZVideoFrame.h"
#include "./../Utils/Define.h"
PartnerVideoRender::PartnerVideoRender() : frameCount(0) {
    startTime = std::chrono::steady_clock::now();
}

PartnerVideoRender::~PartnerVideoRender() {}

void PartnerVideoRender::setVideoFrameLabel(
    std::shared_ptr<YuvWidget> &widget) {
    _partnerRenderWidget = widget;
}

void PartnerVideoRender::render(const std::shared_ptr<void> &frame,
                                ImageFormat format) {
    if (format == ImageFormat::YUV420) {
        auto framePtr = std::static_pointer_cast<ZVideoFrame>(frame);
        if (framePtr->yuv420pData.size() != 0) {
            // qDebug() << "render frame" << framePtr.get()->timestamp
            //          << framePtr.get()->width << framePtr.get()->height;
            _partnerRenderWidget->setFrameData(
                framePtr.get()->yuv420pData, framePtr->width, framePtr->height);
            getInfo(framePtr->width, framePtr->height);
        }
    } else {
        qDebug() << "Unsupported image format";
        return;
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