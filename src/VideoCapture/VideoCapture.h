#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <algorithm>


typedef unsigned char uchar;
struct ZVideoFrame
{
    const uchar *yuv420pData;
    int width;
    int height;

    ZVideoFrame() : yuv420pData(nullptr) {}

    ZVideoFrame(const uchar *data, int width, int height) : yuv420pData(data), width(width), height(height) {}

    ZVideoFrame(const ZVideoFrame &other) : yuv420pData(other.yuv420pData) {}

    ZVideoFrame &operator=(const ZVideoFrame &other)
    {
        if (this != &other)
        {
            yuv420pData = other.yuv420pData;
        }
        return *this;
    }

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
