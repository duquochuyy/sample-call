#ifndef PARTNERVIDEORENDER_H
#define PARTNERVIDEORENDER_H

#include <mutex>

#include "./CustomWidget/Yuv420Widget.h"
#include "./VideoRender.h"

class PartnerVideoRender : public VideoRender {
   public:
    PartnerVideoRender();
    ~PartnerVideoRender();
    void setVideoFrameLabel(std::shared_ptr<YuvWidget> &widget) override;
    void render(const std::shared_ptr<void> &frame,
                ImageFormat format) override;

   private:
    void getInfo(int width, int height);

   private:
    std::shared_ptr<YuvWidget> _partnerRenderWidget;
    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif
