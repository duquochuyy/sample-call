#include "./PartnerVideoRender.h"

PartnerVideoRender::PartnerVideoRender() : frameCount(0)
{
}

PartnerVideoRender::~PartnerVideoRender()
{
    delete _label;
}

void PartnerVideoRender::setVideoFrameLabel(QLabel *&label)
{
    _label = label;
}

void PartnerVideoRender::render(const QImage &image)
{
    if (image.isNull()) // Kiểm tra xem QImage có trống không
    {
        qDebug() << "Empty image received.";
        return;
    }

    if (_label != nullptr)
    {
        _label->setPixmap(QPixmap::fromImage(image).scaled(_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        getInfo(image.width(), image.height());
    }
};

void PartnerVideoRender::getInfo(int width, int height)
{
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0)
    {
        int fps = frameCount.load() / elapsedSeconds.count();
        frameCount = 0;
        startTime = currentTime;

        if (_callback != nullptr)
            _callback->onShowInfoPartnerFps(fps);
    }
}