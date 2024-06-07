#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <algorithm>
#include <stdio.h>
#include <cstring>
#include "./../utils/utils.h"

typedef unsigned char uchar;
struct ZVideoFrame
{
    uchar *yuv420pData;
    int width;
    int height;
    uint64_t timestamp;

    // Constructor mặc định
    ZVideoFrame() : yuv420pData(nullptr), width(0), height(0), timestamp(0) {}

    // Constructor với tham số
    ZVideoFrame(const uchar *data, int width, int height, uint64_t timestamp = 0)
        : width(width), height(height), timestamp(timestamp)
    {
        int dataSize = width * height * 3 / 2;
        yuv420pData = new uchar[dataSize];
        std::memcpy(yuv420pData, data, dataSize);
    }

    // Copy constructor
    ZVideoFrame(const ZVideoFrame &other)
        : width(other.width), height(other.height), timestamp(other.timestamp)
    {
        int dataSize = other.width * other.height * 3 / 2;
        yuv420pData = new uchar[dataSize];
        std::memcpy(yuv420pData, other.yuv420pData, dataSize);
    }

    // Operator=
    ZVideoFrame &operator=(const ZVideoFrame &other)
    {
        if (this != &other)
        {
            delete[] yuv420pData; // Giải phóng bộ nhớ cũ
            width = other.width;
            height = other.height;
            timestamp = other.timestamp;
            int dataSize = other.width * other.height * 3 / 2;
            yuv420pData = new uchar[dataSize];
            std::memcpy(yuv420pData, other.yuv420pData, dataSize);
        }
        return *this;
    }

    // Destructor
    ~ZVideoFrame()
    {
        delete[] yuv420pData;
    }
};

class VideoCapture
{
public:
    class Callback
    {
    public:
        virtual void onNewVideoFrame(const ZVideoFrame &frame) = 0;
    };

public:
    VideoCapture(){};
    virtual ~VideoCapture() = default;

    void registerCallback(Callback *callback);

    virtual void setSetting(int width, int height) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

protected:
    Callback *_callback;
};

#endif
