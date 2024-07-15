#ifndef ENCODEAV1_H
#define ENCODEAV1_H

#include <stdint.h>

#include <fstream>
#include <iostream>
#include <string>

#include "./../Common/ZEncodedFrame.h"
#include "./../Utils/Define.h"
#include "./EncodeFrame.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include "EbSvtAv1.h"
#include "EbSvtAv1Dec.h"
#include "EbSvtAv1Enc.h"

#ifdef __cplusplus
}
#endif

struct SvtAv1Context {
    EbComponentType *svtHandle;
    EbSvtAv1EncConfiguration encParams;
    EbSvtAv1DecConfiguration decParams;
    EbBufferHeaderType *outputStreamBuffer;
    EbBufferHeaderType *inputPictureBuffer;
};

class EncodeAV1 : public EncodeFrame {
   public:
    EncodeAV1(int width = WIDTH, int height = HEIGHT, int bitrate = BITRATE);
    ~EncodeAV1();

    void encode(const ZVideoFrame &frame, ZEncodedFrame &encodedFrame) override;
    void disconnect() override;

   private:
    void getInfo(int width, int height);

   private:
    SvtAv1Context svtCtx;
    EbErrorType res;
    EbSvtIOFormat *frameBuffer;

    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif  // ENCODEAV1_H
