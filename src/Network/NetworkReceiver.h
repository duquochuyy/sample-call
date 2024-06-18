#ifndef NETWORKRECEIVER_H
#define NETWORKRECEIVER_H

#include <qDebug>
#include <QLabel>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

#include "./../VideoCapture/VideoCapture.h"

#define PACKER_SIZE 1024

class NetworkReceiver
{
public:
    class Callback
    {
    public:
        virtual void onAcceptedConnection(std::string partnerIP, int partnerPort) = 0;
        virtual void onReceiveFrame(const std::vector<uchar> &encodedData, const uint64_t timestamp) = 0;
        virtual void onRequestDisconnect() = 0;
        virtual void onRenderInfoReceiver(int width = 1280, int height = 720, int fps = 30, double bitrate = 1.0) = 0;
    };

public:
    NetworkReceiver(int port);
    ~NetworkReceiver();
    void registerCallback(Callback *callback);
    void startListening();
    void disconnect();

private:
    void handleRequestConnect();
    void receiveData();
    void handleConnectBack(int partnerPort);
    void handleRequestDisconnect();
    void getInfoReceive(int width, int height);

private:
    Callback *_callback;

    int _port;
    int receiver_fd;
    int sender_sock;
    std::unordered_map<uint64_t, std::map<int, std::vector<uchar>>> bufferFrames;

    std::thread listenThread;
    std::thread receiveThread;
    std::thread connectBackThread;

    std::mutex socketMutex;
    std::mutex callbackMutex;

    std::atomic<uint64_t> totalBytesReceive;
    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif
