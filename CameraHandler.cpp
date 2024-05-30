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
      captureTimer(new QTimer(this))
{
    connect(imageCapture, &QCameraImageCapture::imageCaptured, this, &CameraHandler::onImageCaptured);
    connect(captureTimer, &QTimer::timeout, this, &CameraHandler::captureFrame);

    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    lastCapturedFrame = documentsPath + "/current_frame.jpg";
}

void CameraHandler::startCamera()
{
    camera->setCaptureMode(QCamera::CaptureStillImage);
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);

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
    imageCapture->capture(lastCapturedFrame);
    emit newFrameCaptured();
}

void CameraHandler::onImageCaptured(int id, const QImage &preview)
{
    // hàm này tạm thời ko thể sử dụng
    qDebug() << "capture image: " << id;
    if (preview.save(lastCapturedFrame)) {
        emit newFrameCaptured();
        qDebug() << "Image captured and saved to" << lastCapturedFrame;
    } else {
        qDebug() << "Failed to save image to" << lastCapturedFrame;
    }
}

QString CameraHandler::getLastCapturedFrame() const
{
    return lastCapturedFrame;
}
