#ifndef DECODEFRAME_H
#define DECODEFRAME_H

#include <QDebug>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <string>
#include "x264.h"
#include "./../VideoCapture/VideoCapture.h"
#include "./../Codec/EncodeFrame.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

class DecodeFrame
{
public:
    DecodeFrame(int width, int height);
    ~DecodeFrame();

    ZVideoFrame decodeH264ToYUV420(const std::vector<uint8_t> &encodedData, const uint64_t timestamp);

private:
    const AVCodec *codec;
    AVCodecContext *codecContext;
    AVFrame *frame;
    AVPacket *packet;
};

#endif