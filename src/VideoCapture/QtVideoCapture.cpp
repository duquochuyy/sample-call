#include "QtVideoCapture.h"
#include <QCameraInfo>

QtVideoCapture::QtVideoCapture(QObject *parent) : QObject(parent),
                                                  camera(new QCamera(QCameraInfo::defaultCamera())),
                                                  videoSurface(new QTVideoSurface(this)),
                                                  frameCount(0)
{
    QObject::connect(camera, &QCamera::stateChanged, this, &QtVideoCapture::onCameraStateChanged);
    QObject::connect(camera, QOverload<QCamera::Error>::of(&QCamera::error), this, &QtVideoCapture::onCameraError);
    QObject::connect(videoSurface, &QTVideoSurface::frameCapturedRawFormat, this, &QtVideoCapture::onFrameCapturedRawFormat);

    camera->setViewfinder(videoSurface);
}

QtVideoCapture::~QtVideoCapture()
{
    delete camera;
    delete videoSurface;
}

void QtVideoCapture::setSetting(int width, int height)
{
    QCameraViewfinderSettings viewfinderSettings;
    viewfinderSettings.setResolution(width, height);
    camera->setViewfinderSettings(viewfinderSettings);
}

void QtVideoCapture::start()
{
    qDebug() << "QtVideoCapture::start";
    camera->start();
}

void QtVideoCapture::stop()
{
    camera->stop();
}

void QtVideoCapture::onFrameCapturedRawFormat(const ZRootFrame &frame)
{
    getInfo(frame.width, frame.height);
    if (_callback != nullptr)
    {
        _callback->onNewVideoFrameRawFormat(frame);
    }
}

void QtVideoCapture::onCameraStateChanged(QCamera::State state)
{
    qDebug() << "Camera state changed to:" << state;
    if (state == QCamera::ActiveState)
    {
        qDebug() << "Camera is active";
    }
    else if (state == QCamera::UnloadedState)
    {
        qDebug() << "Camera is unloaded";
    }
    else if (state == QCamera::LoadedState)
    {
        qDebug() << "Camera is loaded";
    }
    else if (state == QCamera::StartingStatus)
    {
        qDebug() << "Camera is starting";
    }
    else if (state == QCamera::StoppingStatus)
    {
        qDebug() << "Camera is stopping";
    }
}

void QtVideoCapture::onCameraError(QCamera::Error error)
{
    qDebug() << "Camera error:" << error;
    if (error == QCamera::NoError)
    {
        qDebug() << "No error";
    }
    else if (error == QCamera::CameraError)
    {
        qDebug() << "General camera error";
    }
    else if (error == QCamera::InvalidRequestError)
    {
        qDebug() << "Invalid request error";
    }
    else if (error == QCamera::ServiceMissingError)
    {
        qDebug() << "Service missing error";
    }
    else if (error == QCamera::NotSupportedFeatureError)
    {
        qDebug() << "Feature not supported error";
    }
}

void QtVideoCapture::getInfo(int width, int height)
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
            _callback->onShowInfoCapture(width, height, fps);
    }
}
