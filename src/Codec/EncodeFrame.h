#ifndef ENCODEFRAME_H
#define ENCODEFRAME_H

#include "./../Common/ZEncodedFrame.h"
class EncodeFrame {
   public:
    class Callback {
       public:
        virtual void onShowInfoEncode(int width, int height, int fps) = 0;
    };

   public:
    EncodeFrame(int width = WIDTH, int height = HEIGHT, int bitrate = BITRATE) {
    }
    virtual ~EncodeFrame() = default;
    void registerCallback(Callback *callback);
    virtual void encode(const ZVideoFrame &frame,
                        ZEncodedFrame &encodedFrame) = 0;
    virtual void disconnect() = 0;

   protected:
    Callback *_callback;
};

#endif