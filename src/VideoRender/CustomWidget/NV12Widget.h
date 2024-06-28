#ifndef NV12WIDGET_H
#define NV12WIDGET_H

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

class NV12Widget : public QOpenGLWidget,
                   protected QOpenGLFunctions,
                   public YuvWidget {
   public:
    explicit NV12Widget(QWidget *parent = nullptr);
    ~NV12Widget();

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

#endif  // NV12WIDGET_H
