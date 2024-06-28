QT       += core gui widgets multimedia multimediawidgets network opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

QMAKE_CXXFLAGS += -arch arm64
QMAKE_LFLAGS += -arch arm64

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

ROOT_DIR = $$PWD/../..

INCLUDEPATH += \
    $$ROOT_DIR/libs/x264 \
    /opt/homebrew/opt/ffmpeg/include

LIBS += \
    -L$$ROOT_DIR/libs/x264 -lx264 \
    -L/opt/homebrew/opt/ffmpeg/lib -lavcodec -lavformat -lavutil -lswscale

HEADERS += \
    $$ROOT_DIR/libs/x264/x264.h

SOURCES += \
    $$ROOT_DIR/src/main.cpp \
    $$ROOT_DIR/src/CallController/CallController.cpp \
    $$ROOT_DIR/src/VideoCapture/QtVideoCapture.cpp \
    $$ROOT_DIR/src/VideoCapture/VideoCapture.cpp \
    $$ROOT_DIR/src/VideoCapture/QtVideoSurface.cpp \
    $$ROOT_DIR/src/VideoRender/QtVideoRender.cpp \
    $$ROOT_DIR/src/VideoRender/PartnerVideoRender.cpp \
    $$ROOT_DIR/src/VideoRender/VideoRender.cpp \
    $$ROOT_DIR/src/Network/NetworkReceiver.cpp \
    $$ROOT_DIR/src/Network/NetworkSender.cpp \
    $$ROOT_DIR/src/Codec/EncodeFrame.cpp \
    $$ROOT_DIR/src/Codec/DecodeFrame.cpp \
    $$ROOT_DIR/src/Utils/Utils.cpp \
    $$ROOT_DIR/src/Utils/TimeTracker.cpp \
    $$ROOT_DIR/src/Utils/ThreadSafeQueue.cpp \
    $$ROOT_DIR/src/Utils/Convert.cpp \
    $$ROOT_DIR/src/VideoRender/CustomWidget/Yuv420Widget.cpp \
    $$ROOT_DIR/src/VideoRender/CustomWidget/NV12Widget.cpp \
    $$ROOT_DIR/src/MainWindow.cpp

HEADERS += \
    $$ROOT_DIR/src/CallController/CallController.h \
    $$ROOT_DIR/src/VideoCapture/QtVideoCapture.h \
    $$ROOT_DIR/src/VideoCapture/VideoCapture.h \
    $$ROOT_DIR/src/VideoCapture/QtVideoSurface.h \
    $$ROOT_DIR/src/VideoRender/QtVideoRender.h \
    $$ROOT_DIR/src/VideoRender/PartnerVideoRender.h \
    $$ROOT_DIR/src/VideoRender/VideoRender.h \
    $$ROOT_DIR/src/Network/NetworkReceiver.h \
    $$ROOT_DIR/src/Network/NetworkSender.h \
    $$ROOT_DIR/src/Codec/EncodeFrame.h \
    $$ROOT_DIR/src/Codec/DecodeFrame.h \
    $$ROOT_DIR/src/Utils/Utils.h \
    $$ROOT_DIR/src/Utils/TimeTracker.h \
    $$ROOT_DIR/src/Utils/ThreadSafeQueue.h \
    $$ROOT_DIR/src/Utils/Convert.h \
    $$ROOT_DIR/src/VideoRender/CustomWidget/YuvWidget.h \
    $$ROOT_DIR/src/VideoRender/CustomWidget/Yuv420Widget.h \
    $$ROOT_DIR/src/VideoRender/PartnerVideoRender.h \
    $$ROOT_DIR/src/MainWindow.h

FORMS += \
    $$ROOT_DIR/src/View/MainWindow.ui

RESOURCES += \
    $$ROOT_DIR/projects/QT_MAC/resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    $$ROOT_DIR/projects/QT_MAC/Info.plist
