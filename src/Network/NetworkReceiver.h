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
#define MAX_FRAMES 150

class NetworkReceiver
{
public:
    class Callback
    {
    public:
        virtual void onAcceptedConnection(std::string partnerIP, int partnerPort) = 0;
        virtual void onReceiveFrame(const ZVideoFrame &frame) = 0;
        virtual void onRequestDisconnect() = 0;
    };

public:
    NetworkReceiver(int port);
    ~NetworkReceiver();
    void registerCallback(Callback *callback);
    void startListening();
    void setLabelInfoReceive(QLabel *width, QLabel *height, QLabel *FPS, QLabel *bitrate);
    void disconnect();

private:
    void handleRequestConnect();
    void receiveData();
    void handleConnectBack(int partnerPort);
    void testShowImage(const uchar *yuv420pData, int width, int height, QString fileName);
    void renderInfoReceive(int width = 640, int height = 480, int FPS = 30, double bitrate = 100.0);
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

    // for render info
    QLabel *_labelWidth;
    QLabel *_labelHeight;
    QLabel *_labelFPS;
    QLabel *_labelBitrate;
};

#endif
