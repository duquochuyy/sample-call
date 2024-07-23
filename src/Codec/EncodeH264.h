#ifndef ENCODEH264_H
#define ENCODEH264_H

#include <stdint.h>

#include <iostream>
#include <string>

#include "./../Common/ZEncodedFrame.h"
#include "./../Utils/Define.h"
#include "./EncodeFrame.h"
#include "x264.h"

class EncodeH264 : public EncodeFrame {
   public:
    EncodeH264(int width = WIDTH, int height = HEIGHT, int bitrate = BITRATE);
    ~EncodeH264();

    void encode(const ZVideoFrame &frame, ZEncodedFrame &encodedFrame) override;
    void disconnect() override;

   private:
    void getInfo(int width, int height);

   private:
    x264_t *encoder;
    x264_param_t param;
    x264_picture_t pic;
    x264_picture_t pic_out;

    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif