#ifndef YUV420WIDGET_H
#define YUV420WIDGET_H

#include <QException>
#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QScopedPointer>
#include <vector>

class Yuv420Widget : public QOpenGLWidget, protected QOpenGLFunctions {
public:
    explicit Yuv420Widget(QWidget *parent = nullptr);
    ~Yuv420Widget();

    void setFrameData(const std::vector<unsigned char> &data, int frameWidth, int frameHeight);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    struct YuvWidgetImpl;
    QScopedPointer<YuvWidgetImpl> impl;
};

class OpenGlException : public QException {
public:
    void raise() const { throw *this; }
    OpenGlException *clone() const {
        return new OpenGlException(*this);
    }
};

#endif // YUV420WIDGET_H
