#include "Yuv420Widget.h"

#include "./../../Utils/Define.h"

struct Yuv420Widget::YuvWidgetImpl {
    YuvWidgetImpl() : mFrameSize(0) {}

    std::vector<unsigned char> mBufYuv;
    int mFrameSize;

    std::unique_ptr<QOpenGLShader> mVShader;
    std::unique_ptr<QOpenGLShader> mFShader;
    std::unique_ptr<QOpenGLShaderProgram> mShaderProgram;

    std::unique_ptr<QOpenGLTexture> mTextureY;
    std::unique_ptr<QOpenGLTexture> mTextureU;
    std::unique_ptr<QOpenGLTexture> mTextureV;

    GLuint id_y, id_u, id_v;
    int textureUniformY, textureUniformU, textureUniformV;
    GLsizei mVideoW, mVideoH;
};

Yuv420Widget::Yuv420Widget(QWidget *parent)
    : QOpenGLWidget(parent), impl(new YuvWidgetImpl) {}

Yuv420Widget::~Yuv420Widget() {}

void Yuv420Widget::setFrameData(const std::vector<unsigned char> &data,
                                int frameWidth, int frameHeight) {
    impl->mVideoW = frameWidth;
    impl->mVideoH = frameHeight;

    // Allocate memory if not allocated or if size has changed
    int newSize = frameWidth * frameHeight * 3 / 2;  // YUV420 format
    if (impl->mBufYuv.size() != newSize) {
        impl->mBufYuv.resize(newSize);
    }

    // Copy data to internal buffer
    std::memcpy(impl->mBufYuv.data(), data.data(), data.size());

    update();
}

void Yuv420Widget::initializeGL() {
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    impl->mVShader =
        std::make_unique<QOpenGLShader>(QOpenGLShader::Vertex, this);
    const char *vsrc =
        "attribute vec4 vertexIn; \
        attribute vec2 textureIn; \
        varying vec2 textureOut;  \
        void main(void)           \
        {                         \
            gl_Position = vertexIn; \
            textureOut = textureIn; \
        }";

    bool bCompile = impl->mVShader->compileSourceCode(vsrc);
    if (!bCompile) {
        qDebug() << "Error connect vertex shader";
    }

    impl->mFShader =
        std::make_unique<QOpenGLShader>(QOpenGLShader::Fragment, this);
    const char *fsrc =
        "varying vec2 textureOut; \
    uniform sampler2D tex_y; \
    uniform sampler2D tex_u; \
    uniform sampler2D tex_v; \
    void main(void) \
    { \
        vec3 yuv; \
        vec3 rgb; \
        yuv.x = texture2D(tex_y, textureOut).r; \
        yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
        yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
        rgb = mat3( 1,       1,         1, \
                    0,       -0.39465,  2.03211, \
                    1.13983, -0.58060,  0) * yuv; \
        gl_FragColor = vec4(rgb, 1); \
    }";

    bCompile = impl->mFShader->compileSourceCode(fsrc);
    if (!bCompile) {
        qDebug() << "Error connect fragment shader";
    }

    impl->mShaderProgram = std::make_unique<QOpenGLShaderProgram>(this);
    // add fragment shader
    impl->mShaderProgram->addShader(impl->mFShader.get());
    // add vertex shader
    impl->mShaderProgram->addShader(impl->mVShader.get());
    impl->mShaderProgram->bindAttributeLocation("vertexIn",
                                                PROGRAM_VERTEX_ATTRIBUTE);
    impl->mShaderProgram->bindAttributeLocation("textureIn",
                                                PROGRAM_TEXCOORD_ATTRIBUTE);
    impl->mShaderProgram->link();
    impl->mShaderProgram->bind();

    impl->textureUniformY = impl->mShaderProgram->uniformLocation("tex_y");
    impl->textureUniformU = impl->mShaderProgram->uniformLocation("tex_u");
    impl->textureUniformV = impl->mShaderProgram->uniformLocation("tex_v");

    static const GLfloat vertexVertices[] = {
        -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
    };

    static const GLfloat textureVertices[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    };

    glVertexAttribPointer(PROGRAM_VERTEX_ATTRIBUTE, 2, GL_FLOAT, 0, 0,
                          vertexVertices);
    glVertexAttribPointer(PROGRAM_TEXCOORD_ATTRIBUTE, 2, GL_FLOAT, 0, 0,
                          textureVertices);
    glEnableVertexAttribArray(PROGRAM_VERTEX_ATTRIBUTE);
    glEnableVertexAttribArray(PROGRAM_TEXCOORD_ATTRIBUTE);

    impl->mTextureY =
        std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    impl->mTextureU =
        std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    impl->mTextureV =
        std::make_unique<QOpenGLTexture>(QOpenGLTexture::Target2D);
    impl->mTextureY->create();
    impl->mTextureU->create();
    impl->mTextureV->create();

    impl->id_y = impl->mTextureY->textureId();
    impl->id_u = impl->mTextureU->textureId();
    impl->id_v = impl->mTextureV->textureId();

    glClearColor(0.3, 0.3, 0.3, 0.0);
}

void Yuv420Widget::resizeGL(int w, int h) {
    if (h == 0)
        h = 1;
    glViewport(0, 0, w, h);
    update();
}

void Yuv420Widget::paintGL() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, impl->id_y);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, impl->mVideoW, impl->mVideoH,
                 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, impl->mBufYuv.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, impl->id_u);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, impl->mVideoW / 2,
                 impl->mVideoH / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 impl->mBufYuv.data() + impl->mVideoW * impl->mVideoH);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, impl->id_v);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, impl->mVideoW / 2,
                 impl->mVideoH / 2, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 impl->mBufYuv.data() + impl->mVideoW * impl->mVideoH * 5 / 4);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glUniform1i(impl->textureUniformY, 0);
    glUniform1i(impl->textureUniformU, 1);
    glUniform1i(impl->textureUniformV, 2);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}