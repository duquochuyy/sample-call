#include "MainWindow.h"
#include <memory>
#include <mutex>
#include <fstream>
#include <iostream>

#include <QApplication>

#include "./CallController/CallController.h"
#include "mathlib.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
}

// std::mutex fileMutex;

// int getPortNumber(int basePort)
// {
//     std::lock_guard<std::mutex> guard(fileMutex);
//     const char *filename = "/tmp/app_instance_count.txt";
//     std::ifstream infile(filename);
//     int count = 0;

//     if (infile.is_open())
//     {
//         infile >> count;
//         infile.close();
//     }

//     count++;
//     std::ofstream outfile(filename);
//     if (outfile.is_open())
//     {
//         outfile << count;
//         outfile.close();
//     }

//     return basePort + count;
// }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    int port = 8080;
    if (argc > 1)
    {
        port = std::stoi(argv[1]);
    }
    avformat_network_init();
    qDebug() << "FFmpeg version: " << av_version_info();

    std::shared_ptr<CallController> _callController = std::make_shared<CallController>(port);
    MainWindow w(_callController, port);
    w.show();
    return a.exec();
}
