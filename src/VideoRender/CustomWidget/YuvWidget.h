#ifndef YUVWIDGET_H
#define YUVWIDGET_H

#include <QOpenGLWidget>
#include <QWidget>
#include <vector>

class YuvWidget {
   public:
    virtual ~YuvWidget() {};
    virtual void setFrameData(const std::vector<unsigned char> &data,
                              int frameWidth, int frameHeight) = 0;
};

#endif