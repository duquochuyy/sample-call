#ifndef QTVIDEORENDER_H
#define QTVIDEORENDER_H

#include "./VideoRender.h"

class QtVideoRender : public VideoRender
{
public:
    QtVideoRender();
    ~QtVideoRender();

    void setVideoFrameLabel(QLabel *label) override;
    QImage convertYUV420ToRGB(const std::vector<uchar> &yuv420Data, int width, int height) override;
    void render(const std::shared_ptr<ZVideoFrame> &frame) override;

private:
    QLabel *_label;
};

#endif
