#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <stdio.h>

#include <QImage>
#include <algorithm>
#include <cstring>
#include <vector>

#include "./../Common/ZEncodedFrame.h"
#include "./../Common/ZRootFrame.h"
#include "./../Common/ZVideoFrame.h"

class VideoCapture {
  public:
    class Callback {
      public:
        virtual void onNewVideoFrameRawFormat(const ZRootFrame &frame) = 0;
        virtual void onShowInfoCapture(int width, int height, int fps) = 0;
    };

  public:
    VideoCapture() {};
    virtual ~VideoCapture() = default;

    void registerCallback(Callback *callback);

    virtual void setSetting(int width, int height) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

  protected:
    Callback *_callback;
};

#endif
