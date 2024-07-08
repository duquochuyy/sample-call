#ifndef VIDEORENDER_H
#define VIDEORENDER_H

#include "./../Utils/Convert.h"
#include "./../Utils/Define.h"
#include "./../VideoCapture/VideoCapture.h"
#include "./CustomWidget/YuvWidget.h"

class VideoRender {
   public:
    class Callback {
       public:
        virtual void onShowInfoLocalFps(int fps) = 0;
        virtual void onShowInfoPartnerFps(int fps) = 0;
    };

   public:
    VideoRender();
    virtual ~VideoRender() = default;

    void registerCallback(Callback *callback);

    virtual void setVideoFrameLabel(std::shared_ptr<YuvWidget> &widget) = 0;
    virtual void render(const std::shared_ptr<void> &frame,
                        ImageFormat format) = 0;

   protected:
    Callback *_callback;
    std::shared_ptr<Convert> _convert;
};

#endif
