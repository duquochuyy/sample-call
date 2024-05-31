#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <QObject>
#include <QCamera>
#include <QCameraImageCapture>
#include <QTimer>
#include <QString>
#include <QVideoProbe>
#include <QCameraViewfinder>
#include <QCameraViewfinderSettings>

class CameraHandler : public QObject
{
    Q_OBJECT
public:
    explicit CameraHandler(QObject *parent = nullptr);

    void startCamera();
    void stopCamera();

signals:
    void newFrameCaptured(const QImage &frame);

private slots:
    void captureFrame();
    void onImageCaptured(int id, const QVideoFrame &preview);

private:
    QCamera *camera;
    QCameraImageCapture *imageCapture;
    QTimer *captureTimer;
    QCameraViewfinder *viewfinder;
    QCameraViewfinderSettings viewfinderSettings;
};

#endif // CAMERAHANDLER_H
