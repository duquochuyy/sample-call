#include "./QtVideoRender.h"

QtVideoRender::QtVideoRender() : frameCount(0)
{
    startTime = std::chrono::steady_clock::now();
}

QtVideoRender::~QtVideoRender()
{
    delete _label;
    delete _labelWidth;
    delete _labelHeight;
    delete _labelFps;
}

void QtVideoRender::setVideoFrameLabel(QLabel *label)
{
    _label = label;
}

void QtVideoRender::render(const QImage &image)
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
}

void QtVideoRender::getInfo(int width, int height)
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
            _callback->onShowInfoLocalFps(fps);
    }
}
