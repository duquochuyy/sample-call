// utils/utils.h
#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <QImage>
#include <QDebug>
#include "./../VideoCapture/VideoCapture.h"
#include "./../VideoCapture/QtVideoSurface.h"

class Utils
{
public:
    static uint64_t getCurrentTimestamp();
};

#endif // UTILS_H
