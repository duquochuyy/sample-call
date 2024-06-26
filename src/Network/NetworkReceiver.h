#ifndef NETWORKRECEIVER_H
#define NETWORKRECEIVER_H

#include <ctype.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <QDebug>
#include <QLabel>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#include "./../Codec/EncodeFrame.h"
#include "./../Common/ZEncodedFrame.h"
#include "./../Common/ZRootFrame.h"
#include "./../Common/ZVideoFrame.h"

#define PACKER_SIZE 1024

class NetworkReceiver {
   public:
    class Callback {
       public:
        virtual void onAcceptedConnection(std::string partnerIP,
                                          int partnerPort) = 0;
        virtual void onReceiveDataFrame(const std::vector<uchar> &fullFrameData,
                                        uint64_t timestamp) = 0;
        virtual void onRequestDisconnect() = 0;
        virtual void onShowInfoReceive(int fps, int pps, double bitrate) = 0;
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
    void getInfo();

   private:
    Callback *_callback;

    int _port;
    int receiver_fd;
    int sender_sock;
    std::unordered_map<uint64_t, std::map<int, std::vector<uchar>>>
        bufferFrames;

    std::thread listenThread;
    std::thread receiveThread;
    std::thread connectBackThread;

    std::mutex socketMutex;
    std::mutex callbackMutex;

    std::atomic<uint64_t> totalBytesReceive;
    std::atomic<int> frameCount;
    std::atomic<int> packetCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif
