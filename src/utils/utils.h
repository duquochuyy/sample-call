// utils/utils.h
#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <QImage>
#include <QDebug>
#include "./../VideoCapture/VideoCapture.h"
#include "./../VideoCapture/QtVideoSurface.h"

#define LIMITVALUE(x) (x < 0 ? 0 : (x > 255) ? 255 \
                                             : x)

class Utils
{
public:
    static uint64_t getCurrentTimestamp();
    static void processNV12DatatToRGB(const std::vector<uchar> &nv12Data, int width, int height, QImage &image);
    static void processNV12DatatToYUV420(const std::vector<uchar> &nv12Data, int width, int height, std::vector<uchar> &yuv420pData);
    static void convertYUV420ToRGB(const std::vector<uchar> &yuv420Data, int width, int height, QImage &image);
};

#endif // UTILS_H
