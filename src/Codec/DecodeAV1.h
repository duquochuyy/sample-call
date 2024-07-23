#ifndef DECODEAV1_H
#define DECODEAV1_H

#include "./DecodeFrame.h"
#include "./EncodeAV1.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include "EbSvtAv1.h"
#include "EbSvtAv1Dec.h"

#ifdef __cplusplus
}
#endif

class DecodeAV1 : public DecodeFrame {
   public:
    DecodeAV1(int width = WIDTH, int height = HEIGHT);
    ~DecodeAV1();
    void decode(const std::vector<uchar> &encodedData, const uint64_t timestamp,
                std::shared_ptr<ZVideoFrame> &decodedFrame) override;
    void disconnect() override;

   private:
    void getInfo(int width, int height);

   private:
    SvtAv1Context svtCtx;
    EbErrorType res;
    EbSvtIOFormat *frameBuffer;
    EbAV1StreamInfo *streamInfo;
    EbAV1FrameInfo *frameInfo;

    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
};

#endif