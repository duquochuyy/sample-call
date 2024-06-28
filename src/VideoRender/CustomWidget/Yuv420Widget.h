#ifndef YUV420WIDGET_H
#define YUV420WIDGET_H

#include <QDebug>
#include <QException>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QScopedPointer>
#include <vector>

#include "./YuvWidget.h"

class Yuv420Widget : public QOpenGLWidget,
                     protected QOpenGLFunctions,
                     public YuvWidget {
   public:
    explicit Yuv420Widget(QWidget *parent = nullptr);
    ~Yuv420Widget();

    void setFrameData(const std::vector<unsigned char> &data, int frameWidth,
                      int frameHeight) override;

   protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

   private:
    struct YuvWidgetImpl;
    QScopedPointer<YuvWidgetImpl> impl;
};

#endif  // YUV420WIDGET_H
