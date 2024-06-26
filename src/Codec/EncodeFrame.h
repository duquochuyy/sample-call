#ifndef ENCODEFRAME_H
#define ENCODEFRAME_H

#include <stdint.h>

#include <QDebug>
#include <fstream>
#include <iostream>
#include <string>

#include "./../VideoCapture/VideoCapture.h"
#include "x264.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

#define TIMEBASE 90000

class EncodeFrame {
   public:
    class Callback {
       public:
        virtual void onShowInfoEncode(int width, int height, int fps) = 0;
    };

   public:
    EncodeFrame(int width = 1280, int height = 720);
    ~EncodeFrame();

    void registerCallback(Callback *callback);

    void encodeYUV420ToH264(const ZVideoFrame &frame,
                            ZEncodedFrame &encodedFrame);

   private:
    void getInfo(int width, int height);

   private:
    Callback *_callback;
    int _width;
    int _height;

    x264_t *encoder;
    x264_param_t param;
    x264_picture_t pic;
    x264_picture_t pic_out;

    // test decode
    const AVCodec *codec;
    AVCodecContext *codecContext;
    AVFrame *frame;
    AVPacket *packet;

    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif