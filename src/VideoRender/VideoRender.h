#ifndef VIDEORENDER_H
#define VIDEORENDER_H

#include <qDebug>
#include <QImage>
#include <QLabel>
#include <QPixmap>

#include "./../VideoCapture/VideoCapture.h"

class VideoRender
{
public:
    class Callback
    {
    public:
        virtual void onStateChanged() = 0;
    };

public:
    VideoRender(){};
    virtual ~VideoRender() = default;

    void registerCallback(Callback *callback);

    virtual void setVideoFrameLabel(QLabel *label) = 0;
    virtual QImage convertYUV420ToRGB(const std::vector<uchar> &yuv420Data, int width, int height) = 0;
    virtual void render(const std::shared_ptr<ZVideoFrame> &frame) = 0;

protected:
    Callback *_callback;
};

#endif
