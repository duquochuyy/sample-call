#include "QtVideoCapture.h"
#include <QCameraInfo>

QtVideoCapture::QtVideoCapture(QObject *parent) : QObject(parent),
                                                  camera(new QCamera(QCameraInfo::defaultCamera())),
                                                  videoSurface(new QTVideoSurface(this))
{
    QObject::connect(camera, &QCamera::stateChanged, this, &QtVideoCapture::onCameraStateChanged);
    QObject::connect(camera, QOverload<QCamera::Error>::of(&QCamera::error), this, &QtVideoCapture::onCameraError);
    QObject::connect(videoSurface, &QTVideoSurface::frameCaptured, this, &QtVideoCapture::onFrameCaptured);

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
    setSetting(640, 480);
    camera->start();
}

void QtVideoCapture::stop()
{
    camera->stop();
}

void QtVideoCapture::onFrameCaptured(const uchar *yuv420pData, int width, int height, uint64_t timestamp)
{
    ZVideoFrame newFrame(yuv420pData, width, height, timestamp);
    if (_callback != nullptr)
        _callback->onNewVideoFrame(newFrame);
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
