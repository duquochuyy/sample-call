#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include <QDebug>
#include <QLabel>
#include <QImage>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <iostream>

#include "./../VideoCapture/VideoCapture.h"

#define PACKET_SIZE 1024

class NetworkSender
{
public:
    NetworkSender(const int localPort);
    ~NetworkSender();
    bool handleConnectPartner(std::string ip, int port);
    void addNewFrame(const ZVideoFrame &frame);
    void setLabelInfoSend(QLabel *width, QLabel *height, QLabel *FPS, QLabel *bitrate);
    void disconnect();

private:
    void startSending();
    void sendData();
    void testShowImage(uchar *yuv420pData, int width, int height);
    void renderInfoSend(int width = 640, int height = 480, int FPS = 30, double bitrate = 100.0);
    void getInfoSend();

private:
    int localPort;
    int sock;
    bool isSending;
    bool hasNewFrame = false;

    ZVideoFrame currentFrame;
    std::thread sendThread;
    std::mutex frameMutex;
    std::mutex socketMutex;

    std::atomic<uint64_t> totalBytesSent;
    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;

    // for render info
    QLabel *_labelWidth;
    QLabel *_labelHeight;
    QLabel *_labelFPS;
    QLabel *_labelBitrate;
};
#endif
