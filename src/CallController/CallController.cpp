#include "./CallController.h"

CallController::CallController()
{
     _vidCapture.reset(new QtVideoCapture());
     _vidCapture->registerCallback(this);
     _localRender.reset(new QtVideoRender());
     _localRender->registerCallback(this);
}

void CallController::onNewVideoFrame(const ZVideoFrame &frame) {
    _localRender->render(frame);
}

void CallController::onStateChanged() {}

void CallController::setVideoFrameLabel(QLabel *label)
{
    _localRender->setVideoFrameLabel(label);
}

void CallController::startCamera() {
    qDebug() << "startCamera";
    _vidCapture->start();
}

void CallController::stopCamera() {
    _vidCapture->stop();
}
