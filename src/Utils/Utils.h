#ifndef UTILS_H
#define UTILS_H

#include <QDebug>
#include <QImage>
#include <chrono>

#include "./../VideoCapture/QtVideoSurface.h"
#include "./../VideoCapture/VideoCapture.h"

class Utils {
   public:
    static uint64_t getCurrentTimestamp();
};

#endif  // UTILS_H
