#ifndef DECODEFRAME_H
#define DECODEFRAME_H

#include <stdint.h>

#include <QDebug>
#include <fstream>
#include <iostream>
#include <string>

#include "./../Codec/EncodeFrame.h"
#include "./../VideoCapture/VideoCapture.h"
#include "x264.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

class DecodeFrame {
   public:
    class Callback {
       public:
        virtual void onShowInfoDecode(int width, int height, int fps) = 0;
    };

   public:
    DecodeFrame(int width, int height);
    ~DecodeFrame();

    void registerCallback(Callback *callback);

    void decodeH264ToYUV420(const std::vector<uchar> &encodedData,
                            const uint64_t timestamp,
                            std::shared_ptr<ZVideoFrame> &decodedFrame);

   private:
    void getInfo(int width, int height);

   private:
    Callback *_callback;
    const AVCodec *codec;
    AVCodecContext *codecContext;
    AVFrame *frame;
    AVPacket *packet;

    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif