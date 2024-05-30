#ifndef CAMERAHANDLER_H
#define CAMERAHANDLER_H

#include <QObject>
#include <QCamera>
#include <QCameraImageCapture>
#include <QTimer>
#include <QString>

class CameraHandler : public QObject
{
    Q_OBJECT
public:
    explicit CameraHandler(QObject *parent = nullptr);

    void startCamera();
    void stopCamera();

    QString getLastCapturedFrame() const;

signals:
    void newFrameCaptured();

private slots:
    void captureFrame();
    void onImageCaptured(int id, const QImage &preview);

private:
    QCamera *camera;
    QCameraImageCapture *imageCapture;
    QTimer *captureTimer;
    QString lastCapturedFrame;
};

#endif // CAMERAHANDLER_H
