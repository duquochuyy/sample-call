#ifndef QTVIDEOCAPTURE_H
#define QTVIDEOCAPTURE_H

#include <QObject>
#include <QCamera>

#include "./VideoCapture.h"
#include "./QtVideoSurface.h"

class QtVideoCapture : public QObject, public VideoCapture
{
     Q_OBJECT
public:
     QtVideoCapture(QObject *parent = nullptr);
     ~QtVideoCapture();

     void setSetting(int width, int height) override;

     void start() override;

     void stop() override;

private slots:
     void onCameraStateChanged(QCamera::State state);
     void onCameraError(QCamera::Error error);
     void onFrameCaptured(const uchar * yuv420pData, int width, int );

private:
     QCamera *camera;

     QTVideoSurface *videoSurface;
};

#endif
