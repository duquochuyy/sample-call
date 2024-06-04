#ifndef CALLCONTROLLER_H
#define CALLCONTROLLER_H

#include <memory>
#include <QLabel>

#include "./../VideoCapture/QtVideoCapture.h"
#include "./../VideoCapture/VideoCapture.h"
#include "./../VideoRender/VideoRender.h"
#include "./../VideoRender/QtVideoRender.h"


class CallController : public VideoCapture::Callback,
                       public VideoRender::Callback
{
public:
    CallController();
//    CallController(std::shared_ptr<CallController> callController) {};

    // for capture
    void onNewVideoFrame(const ZVideoFrame &frame) override;

    // for render
    void onStateChanged() override;
    void setVideoFrameLabel(QLabel *label);

public:
    void startCamera();
    void stopCamera();

private:
    std::shared_ptr<VideoCapture> _vidCapture;
    std::shared_ptr<VideoRender> _localRender;
};

#endif
