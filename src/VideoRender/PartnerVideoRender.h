#ifndef PARTNERVIDEORENDER_H
#define PARTNERVIDEORENDER_H

#include "./VideoRender.h"
#include <mutex>

class PartnerVideoRender : public VideoRender
{
public:
    PartnerVideoRender();
    ~PartnerVideoRender();
    void setVideoFrameLabel(QLabel *label) override;
    QImage convertYUV420ToRGB(const uchar *yuv240Data, int width, int height) override;
    void render(const ZVideoFrame &frame) override;

private:
    QLabel *_label;
    std::mutex fileMutex;
};

#endif
