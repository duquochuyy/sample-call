#ifndef DECODEFRAME_H
#define DECODEFRAME_H

#include <stdint.h>

#include <fstream>
#include <iostream>
#include <string>

#include "./../Common/ZVideoFrame.h"

class DecodeFrame {
   public:
    class Callback {
       public:
        virtual void onShowInfoDecode(int width, int height, int fps) = 0;
    };

   public:
    explicit DecodeFrame(int width = WIDTH, int height = HEIGHT) {}
    virtual ~DecodeFrame() = default;

    void registerCallback(Callback *callback);

    virtual void decode(const std::vector<uchar> &encodedData,
                        const uint64_t timestamp,
                        std::shared_ptr<ZVideoFrame> &decodedFrame) = 0;

    virtual void disconnect() = 0;

   protected:
    Callback *_callback;
};

#endif