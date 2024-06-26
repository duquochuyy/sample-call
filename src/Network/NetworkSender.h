#ifndef NETWORKSENDER_H
#define NETWORKSENDER_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <x264.h>

#include <QDebug>
#include <QImage>
#include <QLabel>
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

#include "./../Codec/EncodeFrame.h"
#include "./../Common/ZEncodedFrame.h"
#include "./../Common/ZRootFrame.h"
#include "./../Common/ZVideoFrame.h"

#define PACKET_SIZE 1024

class NetworkSender {
  public:
    class Callback {
      public:
        virtual void onShowInfoSend(int fps, int pps, double bitrate) = 0;
    };

  public:
    NetworkSender(const int localPort);
    ~NetworkSender();
    void registerCallback(Callback *callback);
    bool handleConnectPartner(std::string ip, int port);
    void addNewEncodedFrame(const ZEncodedFrame &encodedFrame);
    void disconnect();

  private:
    void startSending();
    void sendData();
    void getInfo();

  private:
    Callback *_callback;

    int localPort;
    int sock;
    bool isSending;
    bool hasNewFrame = false;
    ZEncodedFrame currentEncodedFrame;

    std::thread sendThread;
    std::mutex encodedFrameMutex;

    std::atomic<uint64_t> totalBytesSent;
    std::atomic<int> frameCount;
    std::atomic<int> packetCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};
#endif
