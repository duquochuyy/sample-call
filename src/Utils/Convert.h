#ifndef CONVERT_H
#define CONVERT_H

#include <QDebug>
#include <QImage>
#include <chrono>

#include "./../VideoCapture/QtVideoSurface.h"
#include "./../VideoCapture/VideoCapture.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif

#define LIMITVALUE(x) (x < 0 ? 0 : (x > 255) ? 255 : x)
#define GETRGB(R, G, B) (R << 16 | G << 8 | B)

class Convert {
   public:
    Convert();
    ~Convert();

    void convertNV12DatatToRGB(const std::vector<uchar> &nv12Data, int width,
                               int height, QImage &image);
    void convertNV12DatatToYUV420(const std::vector<uchar> &nv12Data, int width,
                                  int height, std::vector<uchar> &yuv420pData);
    void convertYUV420ToRGB(const std::vector<uchar> &yuv420Data, int width,
                            int height, QImage &image);
    void resizeNV12DataCapture(const std::vector<uchar> &inputNv12Data,
                               int inputWidth, int inputHeight,
                               std::vector<uchar> &outputNv12Data,
                               int outputWidth, int outputHeight);

   private:
    // Khởi tạo các biến ffmpeg
    AVFrame *srcFrame;
    AVFrame *dstFrame;
    SwsContext *swsCtx;
};

#endif