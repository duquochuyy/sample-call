#include "CameraHandler.h"
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QCameraInfo>

CameraHandler::CameraHandler(QObject *parent)
    : QObject(parent),
      camera(new QCamera(QCameraInfo::defaultCamera())),
      imageCapture(new QCameraImageCapture(camera, this)),
      captureTimer(new QTimer(this)),
      viewfinder(new QCameraViewfinder())
{
    connect(captureTimer, &QTimer::timeout, this, &CameraHandler::captureFrame);
//    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &CameraHandler::onImageCaptured);
    connect(imageCapture, &QCameraImageCapture::imageAvailable, this, &CameraHandler::onImageCaptured);

    // setting view finder
//    viewfinder->show();
//    camera->setViewfinder(viewfinder);
    viewfinderSettings.setPixelFormat(QVideoFrame::Format_YUV420P);
//    viewfinderSettings.setResolution(640, 480);
    camera->setViewfinderSettings(viewfinderSettings);

    // set format
    camera->setCaptureMode(QCamera::CaptureStillImage);
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
    QList<QVideoFrame::PixelFormat> supportedFormats = imageCapture->supportedBufferFormats();
    if (supportedFormats.contains(QVideoFrame::Format_YUV420P)) {
        imageCapture->setBufferFormat(QVideoFrame::Format_YUV420P);
        qDebug() << "Buffer format set to YUV420P";
    } else {
        qDebug() << "YUV420P format not supported, supported formats are:" << supportedFormats;
    }
    imageCapture->setBufferFormat(QVideoFrame::Format_YUV420P);

    qDebug() << imageCapture->bufferFormat();
}

void CameraHandler::startCamera()
{
    camera->start();
    captureTimer->start(1000 / 30); // Capture 30 frames per second
    qDebug() << "Camera started";
}

void CameraHandler::stopCamera()
{
    captureTimer->stop();
    camera->stop();
    qDebug() << "Camera stopped";
}

void CameraHandler::captureFrame()
{
    imageCapture->capture();
}

void CameraHandler::onImageCaptured(int id, const QVideoFrame &preview)
{
    qDebug() << "Frame format:" << preview.pixelFormat();
    emit newFrameCaptured(preview.image());
}

