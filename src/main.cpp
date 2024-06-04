#include "MainWindow.h"
#include <memory>

#include <QApplication>

#include "./CallController/CallController.h"

// struct ZVideoFrame {
// };

// class VideoCapture {
// public:
//     class Callback {
//     public:
//         virtual void onNewVideoFrame(const ZVideoFrame& frame) = 0;
//     };

// public:
//     void registerCallback(Callback* callback) {
//         _callback = callback;
//     }

//     virtual void setSetting() = 0;
//     virtual void start() = 0;
//     virtual void stop() = 0;

// protected:
//     Callback* _callback;
// };

// class QtVideoCapture : public VideoCapture{
// public:
//     virtual void setSetting() override {
//     }

//     virtual void start() override {
//         ZVideoFrame a;
//         if (_callback != nullptr)
//             _callback->onNewVideoFrame(a);
//     }

//     virtual void stop() override {
//     }
// };

// class VideoRender {
// public:
//     class Callback {
//     public:
//         virtual void onStateChanged() = 0;
//     };
// public:
//     void registerCallback(Callback* callback) {
//         _callback = callback;
//     }

//     virtual void setup() = 0;
//     virtual void render() = 0;

// protected:
//     Callback* _callback;
// };

// class QtVideoRender : public VideoRender {

// public:
//     virtual void setup() override {}
//     virtual void render() override {
//         if (_callback)
//             _callback->onStateChanged();
//     }
// };

// class CallController :
// public VideoCapture::Callback,
// public VideoRender::Callback{
// public:
//     CallController() {
//         _vidCapture.reset(new QtVideoCapture());
//         _vidCapture->registerCallback(this);
//         _localRender.reset(new QtVideoRender());
//         _remoteRender.reset(new QtVideoRender());
//     }

//     virtual void onNewVideoFrame(const ZVideoFrame& frame) override;
//     virtual void onStateChanged() override;

// private:
//     std::shared_ptr<VideoCapture> _vidCapture;
//     std::shared_ptr<VideoRender >_localRender;
//     std::shared_ptr<VideoRender> _remoteRender;
// };


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    std::shared_ptr<CallController> _callController;
    MainWindow w(_callController);
    w.show();
    return a.exec();
}
