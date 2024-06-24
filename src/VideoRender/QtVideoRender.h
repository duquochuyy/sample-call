#ifndef QTVIDEORENDER_H
#define QTVIDEORENDER_H

#include "./VideoRender.h"

class QtVideoRender : public VideoRender
{
public:
    QtVideoRender();
    ~QtVideoRender();

    void setVideoFrameLabel(QLabel *&label) override;
    void render(const QImage &image) override;

private:
    void getInfo(int width, int height);

private:
    QLabel *_label;
    // for show info
    QLabel *_labelWidth;
    QLabel *_labelHeight;
    QLabel *_labelFps;

    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif
