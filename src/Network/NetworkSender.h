#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include <QDebug>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>

#include "./../VideoCapture/VideoCapture.h"

#define PACKET_SIZE 1024

class NetworkSender
{
public:
    NetworkSender(const int localPort);
    ~NetworkSender();
    bool handleConnectPartner(std::string ip, int port);
    void sendData(const ZVideoFrame &frame);
    void disconnect();

private:
    int localPort;
    int sock;
    bool isSending;

    ZVideoFrame currentFrame;
    std::thread sendThread;
    std::mutex frameMutex;
    std::mutex socketMutex;

    void startSending();
    void testShowImage(uchar *yuv420pData, int width, int height);
};

#endif
