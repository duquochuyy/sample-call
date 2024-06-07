#ifndef NETWORKRECEIVER_H
#define NETWORKRECEIVER_H

#include <qDebug>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <thread>
#include <vector>

#include "./../VideoCapture/VideoCapture.h"

#define PACKER_SIZE 1024

class NetworkReceiver
{
public:
    class Callback
    {
    public:
        virtual void onAcceptedConnection(std::string partnerIP, int partnerPort) = 0;
        virtual void onReceiveFrame(const ZVideoFrame &frame) = 0;
    };

public:
    NetworkReceiver(int port);
    ~NetworkReceiver();
    void registerCallback(Callback *callback);
    void startListening();
    void disconnect();

private:
    Callback *_callback;

    int _port;
    int receiver_fd;
    int sender_sock;

    std::thread listenThread;
    std::thread receiveThread;
    std::thread connectBackThread;

    std::mutex socketMutex;
    std::mutex callbackMutex;

    void handleRequestConnect();
    void receiveData();
    void handleConnectBack(int partnerPort);
    void testShowImage(const uchar *yuv420pData, int width, int height);
    std::unordered_map<uint64_t, std::map<int, std::vector<uchar>>> bufferFrames;
};

#endif
