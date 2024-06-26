#ifndef VIDEORENDER_H
#define VIDEORENDER_H

#include <QDebug>
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
        virtual void onShowInfoLocalFps(int fps) = 0;
        virtual void onShowInfoPartnerFps(int fps) = 0;
    };

public:
    VideoRender(){};
    virtual ~VideoRender() = default;

    void registerCallback(Callback *callback);

    virtual void setVideoFrameLabel(QLabel *&label) = 0;
    virtual void render(const QImage &image) = 0;

protected:
    Callback *_callback;
};

#endif
