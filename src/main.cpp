#include <QApplication>
#include <QOpenGLContext>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>

#include "./CallController/CallController.h"
#include "MainWindow.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // QSurfaceFormat format;
    // format.setDepthBufferSize(24);
    // format.setStencilBufferSize(8);
    // format.setVersion(3, 3);                        // Request OpenGL
    // version 3.3 format.setProfile(QSurfaceFormat::CoreProfile); // Core
    // Profile QSurfaceFormat::setDefaultFormat(format);

    int port = 8080;
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }
    avformat_network_init();
    qDebug() << "FFmpeg version: " << av_version_info();

    std::unique_ptr<CallController> _callController =
        std::make_unique<CallController>(port);
    MainWindow w(std::move(_callController), port);
    w.show();
    return a.exec();
}
