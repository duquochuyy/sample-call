#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <algorithm>
#include <stdio.h>
#include <cstring>
#include <vector>
#include "./../utils/utils.h"

typedef unsigned char uchar;
struct ZVideoFrame
{
    std::vector<uchar> yuv420pData;
    int width;
    int height;
    uint64_t timestamp;

    // Constructor mặc định
    ZVideoFrame() : width(0), height(0), timestamp(0) {}

    // Constructor với tham số
    ZVideoFrame(const uchar *data, int _width, int _height, uint64_t _timestamp = 0)
        : width(_width), height(_height), timestamp(_timestamp), yuv420pData(data, data + _width * _height * 3 / 2)
    {
    }

    ZVideoFrame(const std::vector<uchar> &dataVec, int _width, int _height, uint64_t _timestamp = 0)
        : yuv420pData(dataVec), width(_width), height(_height), timestamp(_timestamp)
    {
    }

    ZVideoFrame(const std::shared_ptr<uchar> &dataPtr, int _width, int _height, uint64_t _timestamp = 0)
        : width(_width), height(_height), timestamp(_timestamp)
    {
        int dataSize = _width * _height * 3 / 2;
        yuv420pData.resize(dataSize);
        std::memcpy(yuv420pData.data(), dataPtr.get(), dataSize);
    }

    // Copy constructor
    ZVideoFrame(const ZVideoFrame &other)
        : yuv420pData(other.yuv420pData), width(other.width), height(other.height), timestamp(other.timestamp)
    {
    }

    // Assignment operator
    ZVideoFrame &operator=(const ZVideoFrame &other)
    {
        if (this != &other)
        {
            yuv420pData = other.yuv420pData;
            width = other.width;
            height = other.height;
            timestamp = other.timestamp;
        }
        return *this;
    }

    // Destructor
    ~ZVideoFrame() = default;
};
class VideoCapture
{
public:
    class Callback
    {
    public:
        virtual void onNewVideoFrame(const std::shared_ptr<ZVideoFrame> &frame) = 0;
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
