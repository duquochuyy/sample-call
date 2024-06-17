#ifndef ENCODEFRAME_H
#define ENCODEFRAME_H

#include <QDebug>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <string>
#include "x264.h"
#include "./../VideoCapture/VideoCapture.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

#define TIMEBASE 90000

struct ZEncodedFrame
{
    std::vector<uint8_t> encodedData;
    int frameSize;
    int i_nal;
    int width;
    int height;
    uint64_t timestamp;

    // Constructor mặc định
    ZEncodedFrame()
        : frameSize(0), i_nal(0), width(0), height(0), timestamp(0)
    {
    }

    // Constructor với đầy đủ thông tin
    ZEncodedFrame(std::vector<uint8_t> _encodedData, int _frameSize, int _i_nal, int _width, int _height, uint64_t _timestamp)
        : encodedData(std::move(_encodedData)), frameSize(_frameSize), i_nal(_i_nal), width(_width), height(_height), timestamp(_timestamp)
    {
    }

    // Copy constructor
    ZEncodedFrame(const ZEncodedFrame &other)
        : encodedData(other.encodedData), frameSize(other.frameSize), i_nal(other.i_nal), width(other.width), height(other.height), timestamp(other.timestamp)
    {
    }

    // Move constructor
    ZEncodedFrame(ZEncodedFrame &&other) noexcept
        : encodedData(std::move(other.encodedData)), frameSize(other.frameSize), i_nal(other.i_nal), width(other.width), height(other.height), timestamp(other.timestamp)
    {
    }

    // Assignment operator
    ZEncodedFrame &operator=(const ZEncodedFrame &other)
    {
        if (this != &other)
        {
            encodedData = other.encodedData;
            frameSize = other.frameSize;
            i_nal = other.i_nal;
            width = other.width;
            height = other.height;
            timestamp = other.timestamp;
        }
        return *this;
    }

    // Move assignment operator
    ZEncodedFrame &operator=(ZEncodedFrame &&other) noexcept
    {
        if (this != &other)
        {
            encodedData = std::move(other.encodedData);
            frameSize = other.frameSize;
            i_nal = other.i_nal;
            width = other.width;
            height = other.height;
            timestamp = other.timestamp;
        }
        return *this;
    }
};

class EncodeFrame
{
public:
    EncodeFrame(int width = 1280, int height = 720);
    ~EncodeFrame();

    ZEncodedFrame encodeYUV420ToH264(const ZVideoFrame &frame);

private:
    int _width;
    int _height;

    int startTime = 0;
    std::ofstream outFile;

    x264_t *encoder;
    x264_param_t param;
    x264_picture_t pic;
    x264_picture_t pic_out;

    // test decode
    const AVCodec *codec;
    AVCodecContext *codecContext;
    AVFrame *frame;
    AVPacket *packet;
};

#endif