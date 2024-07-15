#include <QApplication>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>

#include "./CallController/CallController.h"
#include "./Utils/Define.h"
#include "MainWindow.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    int port = 8080;
    int codec = CODEC_H264;
    if (argc > 1) {
        port = std::stoi(argv[1]);
        if (argc > 2) {
            std::string codecArg = argv[2];
            if (codecArg == "av1") {
                codec = CODEC_AV1;
            }
        }
    }

    avformat_network_init();
    qDebug() << "FFmpeg version: " << av_version_info();

    std::unique_ptr<CallController> _callController =
        std::make_unique<CallController>(port, codec);
    MainWindow w(std::move(_callController), port, codec);
    w.show();
    return a.exec();
}
