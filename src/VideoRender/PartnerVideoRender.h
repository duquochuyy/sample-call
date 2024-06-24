#ifndef PARTNERVIDEORENDER_H
#define PARTNERVIDEORENDER_H

#include "./VideoRender.h"
#include <mutex>

class PartnerVideoRender : public VideoRender
{
public:
    PartnerVideoRender();
    ~PartnerVideoRender();
    void setVideoFrameLabel(QLabel *&label) override;
    void render(const QImage &image) override;

private:
    void getInfo(int width, int height);

private:
    QLabel *_label;
    std::mutex fileMutex;

    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif
