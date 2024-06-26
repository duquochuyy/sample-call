#include "./YuvWidget.h"

YuvWidget::YuvWidget(QWidget *parent) : QOpenGLWidget(parent) {
    resize(200, 100);
}

YuvWidget::~YuvWidget() {
    makeCurrent();
    glDeleteTextures(1, &m_textureY);
    glDeleteTextures(1, &m_textureU);
    glDeleteTextures(1, &m_textureV);
    doneCurrent();
}

void YuvWidget::initializeGL() {
    initializeOpenGLFunctions();

    qDebug() << "OpenGL Version:"
             << reinterpret_cast<const char *>(glGetString(GL_VERSION));
    qDebug() << "GLSL Version:"
             << reinterpret_cast<const char *>(
                    glGetString(GL_SHADING_LANGUAGE_VERSION));

    // load and config shader
    if (!m_shaderProgram.addShaderFromSourceFile(
            QOpenGLShader::Vertex, ":/src/shaders/yuv_vertex.glsl"))
        qDebug() << "Failed to add vertex shader:" << m_shaderProgram.log();
    if (!m_shaderProgram.addShaderFromSourceFile(
            QOpenGLShader::Fragment, ":/src/shaders/yuv_fragment.glsl"))
        qDebug() << "Failed to add fragment shader:" << m_shaderProgram.log();
    if (!m_shaderProgram.link())
        qDebug() << "Failed to link shader program:" << m_shaderProgram.log();

    // create textures
    glGenTextures(1, &m_textureY);
    glGenTextures(1, &m_textureU);
    glGenTextures(1, &m_textureV);

    glBindTexture(GL_TEXTURE_RECTANGLE, m_textureY);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);

    glBindTexture(GL_TEXTURE_RECTANGLE, m_textureU);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);

    glBindTexture(GL_TEXTURE_RECTANGLE, m_textureV);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        qDebug() << "OpenGL error in initializeGL:" << err;
    }
}

void YuvWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
    m_width = width;
    m_height = height;
}

// void YuvWidget::paintGL()
// {
//     GLenum error = glGetError();
//     if (error != GL_NO_ERROR)
//     {
//         qDebug() << "OpenGL error:" << error;
//     }

//     if (m_yuv420Data.isEmpty())
//         return;
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     makeCurrent();

//     m_shaderProgram.bind();

//     int ySize = m_width * m_height;
//     int uvSize = ySize / 4;
//     qDebug() << "paint" << m_width << m_height << ySize <<
//     m_yuv420Data.size();

//     // Setup Y texture
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_RECTANGLE, m_textureY);
//     glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_LUMINANCE, m_width, m_height, 0,
//     GL_LUMINANCE, GL_UNSIGNED_BYTE, m_yuv420Data.constData());
//     m_shaderProgram.setUniformValue("s_texture_y", 0);

//     // Setup U texture
//     glActiveTexture(GL_TEXTURE1);
//     glBindTexture(GL_TEXTURE_RECTANGLE, m_textureU);
//     glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_LUMINANCE, m_width / 2, m_height
//     / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_yuv420Data.constData() +
//     ySize); m_shaderProgram.setUniformValue("s_texture_u", 1);

//     // Setup V texture
//     glActiveTexture(GL_TEXTURE2);
//     glBindTexture(GL_TEXTURE_RECTANGLE, m_textureV);
//     glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_LUMINANCE, m_width / 2, m_height
//     / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, m_yuv420Data.constData() + ySize
//     + uvSize); m_shaderProgram.setUniformValue("s_texture_v", 2);

//     // tọa độ đỉnh
//     GLfloat vertices[] = {
//         -1.0f, -1.0f,
//         1.0f, -1.0f,
//         -1.0f, 1.0f,
//         1.0f, 1.0f};

//     // tọa độ texture
//     GLfloat texCoords[] = {
//         0.0f, 0.0f,
//         static_cast<GLfloat>(m_width), 0.0f,
//         0.0f, static_cast<GLfloat>(m_height),
//         static_cast<GLfloat>(m_width), static_cast<GLfloat>(m_height)};

//     m_shaderProgram.enableAttributeArray("aVertex");
//     m_shaderProgram.setAttributeArray("aVertex", vertices, 2);

//     m_shaderProgram.enableAttributeArray("aTexCoord");
//     m_shaderProgram.setAttributeArray("aTexCoord", texCoords, 2);

//     // Draw geometry
//     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//     // Cleanup
//     m_shaderProgram.disableAttributeArray("aVertex");
//     m_shaderProgram.disableAttributeArray("aTexCoord");

//     m_shaderProgram.release();
// }

void YuvWidget::paintGL() {
    if (m_yuv420Data.isEmpty())
        return;

    int ySize = m_width * m_height;
    int uvSize = (m_width / 2) * (m_height / 2);

    glClear(GL_COLOR_BUFFER_BIT);

    m_shaderProgram.bind();

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        qDebug() << "OpenGL error:" << err;
    }

    // Setup Y texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_RECTANGLE, m_textureY);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED, m_width, m_height, 0, GL_RED,
                 GL_UNSIGNED_BYTE, m_yuv420Data.constData());
    m_shaderProgram.setUniformValue("s_texture_y", 0);

    while ((err = glGetError()) != GL_NO_ERROR) {
        qDebug() << "OpenGL error after binding Y texture:" << err;
    }

    // Setup U texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_RECTANGLE, m_textureU);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED, m_width / 2, m_height / 2, 0,
                 GL_RED, GL_UNSIGNED_BYTE, m_yuv420Data.constData() + ySize);
    m_shaderProgram.setUniformValue("s_texture_u", 1);

    while ((err = glGetError()) != GL_NO_ERROR) {
        qDebug() << "OpenGL error after binding U texture:" << err;
    }

    // Setup V texture
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_RECTANGLE, m_textureV);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RED, m_width / 2, m_height / 2, 0,
                 GL_RED, GL_UNSIGNED_BYTE,
                 m_yuv420Data.constData() + ySize + uvSize);
    m_shaderProgram.setUniformValue("s_texture_v", 2);

    while ((err = glGetError()) != GL_NO_ERROR) {
        qDebug() << "OpenGL error after binding V texture:" << err;
    }

    // Setup vertex data and attributes
    GLfloat vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat texCoords[] = {0.0f,
                           0.0f,
                           static_cast<GLfloat>(m_width),
                           0.0f,
                           0.0f,
                           static_cast<GLfloat>(m_height),
                           static_cast<GLfloat>(m_width),
                           static_cast<GLfloat>(m_height)};

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(texCoords), nullptr,
                 GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(texCoords),
                    texCoords);

    // Setup vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
                          reinterpret_cast<void *>(sizeof(vertices)));

    // Draw geometry
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    m_shaderProgram.release();

    // Check for OpenGL errors
    while ((err = glGetError()) != GL_NO_ERROR) {
        qDebug() << "OpenGL error after drawing:" << err;
    }
}

void YuvWidget::setFrameData(const QByteArray &yuv420pData, int width,
                             int height) {
    if (yuv420pData.isEmpty() || width <= 0 || height <= 0) {
        qDebug() << "Invalid YUV data";
        return;
    }
    m_width = width;
    m_height = height;
    m_yuv420Data = yuv420pData;
    qDebug() << "gpu process";
    update();
}
