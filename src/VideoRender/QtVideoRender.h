#ifndef QTVIDEORENDER_H
#define QTVIDEORENDER_H

#include "./CustomWidget/NV12Widget.h"
#include "./VideoRender.h"

class QtVideoRender : public VideoRender {
   public:
    QtVideoRender();
    ~QtVideoRender();

    void setVideoFrameLabel(std::shared_ptr<YuvWidget> &widget) override;
    void render(const std::shared_ptr<void> &frame,
                ImageFormat format) override;

   private:
    void getInfo(int width, int height);

   private:
    std::shared_ptr<YuvWidget> _localRenderWidget;
    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif
