#include "MainWindow.h"
#include <memory>
#include <mutex>
#include <fstream>
#include <iostream>

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

std::mutex fileMutex;

int getPortNumber(int basePort)
{
    std::lock_guard<std::mutex> guard(fileMutex);
    const char *filename = "/tmp/app_instance_count.txt";
    std::ifstream infile(filename);
    int count = 0;

    if (infile.is_open())
    {
        infile >> count;
        infile.close();
    }

    count++;
    std::ofstream outfile(filename);
    if (outfile.is_open())
    {
        outfile << count;
        outfile.close();
    }

    return basePort + count;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int port = 8080;
    if (argc > 1)
    {
        port = std::stoi(argv[1]);
    }
    std::shared_ptr<CallController> _callController = std::make_shared<CallController>(port);
    MainWindow w(_callController, port);
    w.show();
    return a.exec();
}
