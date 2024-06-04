#ifndef QTVIDEORENDER_H
#define QTVIDEORENDER_H

#include "./VideoRender.h"

class QtVideoRender : public VideoRender
{
public:
    QtVideoRender();
    ~QtVideoRender();

    void setVideoFrameLabel(QLabel *label) override;
    QImage convertYUV420ToRGB(const uchar *yuv240Data, int width, int height) override;
    void render(const ZVideoFrame &frame) override;

private:
    QLabel* _label;
};

#endif
