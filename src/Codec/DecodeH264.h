#ifndef DECODEH264_H
#define DECODEH264_H

#include <stdint.h>

#include <fstream>

#include "./DecodeFrame.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>

#ifdef __cplusplus
}
#endif

class DecodeH264 : public DecodeFrame {
   public:
    DecodeH264(int width, int height);
    ~DecodeH264();

    void decode(const std::vector<uchar> &encodedData, const uint64_t timestamp,
                std::shared_ptr<ZVideoFrame> &decodedFrame) override;

    void disconnect() override;

   private:
    void getInfo(int width, int height);

   private:
    const AVCodec *codec;
    AVCodecContext *codecContext;
    AVFrame *frame;
    AVPacket *packet;

    std::atomic<int> frameCount;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    std::ofstream output;
};

#endif