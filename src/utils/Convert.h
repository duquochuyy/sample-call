#ifndef CONVERT_H
#define CONVERT_H

#include <QDebug>
#include <QImage>
#include <chrono>

#include "./../VideoCapture/QtVideoSurface.h"
#include "./../VideoCapture/VideoCapture.h"

#define LIMITVALUE(x) (x < 0 ? 0 : (x > 255) ? 255 : x)
#define GETRGB(R, G, B) (R << 16 | G << 8 | B)

class Convert {
   public:
    void processNV12DatatToRGB(const std::vector<uchar> &nv12Data, int width,
                               int height, QImage &image);
    void processNV12DatatToYUV420(const std::vector<uchar> &nv12Data, int width,
                                  int height, std::vector<uchar> &yuv420pData);
    void convertYUV420ToRGB(const std::vector<uchar> &yuv420Data, int width,
                            int height, QImage &image);
};

#endif