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
#include <x264.h>

#include "./../VideoCapture/VideoCapture.h"
#include "./../Codec/EncodeFrame.h"

#define PACKET_SIZE 1024

class NetworkSender
{
public:
    class Callback
    {
    public:
        virtual void onRenderInfoSender(int width, int height, int fps, double bitrate) = 0;
    };

public:
    NetworkSender(const int localPort);
    ~NetworkSender();
    void registerCallback(Callback *callback);
    bool handleConnectPartner(std::string ip, int port);
    void addNewEncodedFrame(const std::shared_ptr<ZEncodedFrame> &encodedFrame);
    void disconnect();

private:
    void startSending();
    void sendData();
    void getInfoSend();

private:
    Callback *_callback;

    int localPort;
    int sock;
    bool isSending;
    bool hasNewFrame = false;
    std::shared_ptr<ZEncodedFrame> currentEncodedFrame;

    std::thread sendThread;
    std::mutex encodedFrameMutex;

    std::atomic<uint64_t> totalBytesSent;
    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};
#endif
