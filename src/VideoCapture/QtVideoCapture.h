#ifndef QTVIDEOCAPTURE_H
#define QTVIDEOCAPTURE_H

#include <QCamera>
#include <QObject>

#include "./QtVideoSurface.h"
#include "./VideoCapture.h"

class QtVideoCapture : public QObject, public VideoCapture {
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
    void onFrameCapturedRawFormat(const ZRootFrame &frame);

   private:
    void getInfo(int width, int height);

   private:
    QCamera *camera;
    QTVideoSurface *videoSurface;

    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif
