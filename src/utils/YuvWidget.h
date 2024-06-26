#ifndef YUVWIDGET_H
#define YUVWIDGET_H

#include <QByteArray>
#include <QDebug>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>

class YuvWidget : public QOpenGLWidget, public QOpenGLFunctions {
    Q_OBJECT

   public:
    explicit YuvWidget(QWidget *parent = nullptr);
    ~YuvWidget();

   public slots:
    void setFrameData(const QByteArray &yuv420pData, int width, int height);

   protected:
    void initializeGL() override;
    void resizeGL(int width, int height) override;
    void paintGL() override;

   private:
    QOpenGLShaderProgram m_shaderProgram;
    GLuint m_textureY = 0;
    GLuint m_textureU = 0;
    GLuint m_textureV = 0;
    int m_width = 0;
    int m_height = 0;
    QByteArray m_yuv420Data;
};

#endif