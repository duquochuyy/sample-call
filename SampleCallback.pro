QT       += core gui widgets multimedia multimediawidgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

QMAKE_CXXFLAGS += -arch arm64
QMAKE_LFLAGS += -arch arm64

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    $$PWD/libs/mathlib \
    $$PWD/libs/x264 \
    /opt/homebrew/opt/ffmpeg/include

LIBS += \
    -L$$PWD/libs/mathlib -lmathlib \
    -L$$PWD/libs/x264 -lx264 \
    -L/opt/homebrew/opt/ffmpeg/lib -lavcodec -lavformat -lavutil -lswscale

HEADERS += \
    $$PWD/libs/mathlib/mathlib.h \
    $$PWD/libs/x264/x264.h

SOURCES += \
    src/main.cpp \
    src/CallController/CallController.cpp \
    src/VideoCapture/QtVideoCapture.cpp \
    src/VideoCapture/VideoCapture.cpp \
    src/VideoCapture/QtVideoSurface.cpp \
    src/VideoRender/QtVideoRender.cpp \
    src/VideoRender/PartnerVideoRender.cpp \
    src/VideoRender/VideoRender.cpp \
    src/Network/NetworkReceiver.cpp \
    src/Network/NetworkSender.cpp \
    src/InfoNetwork/InfoNetwork.cpp \
    src/Codec/EncodeFrame.cpp \
    src/Codec/DecodeFrame.cpp \
    src/utils/utils.cpp \
    src/MainWindow.cpp

HEADERS += \
    src/CallController/CallController.h \
    src/VideoCapture/QtVideoCapture.h \
    src/VideoCapture/VideoCapture.h \
    src/VideoCapture/QtVideoSurface.h \
    src/VideoRender/QtVideoRender.h \
    src/VideoRender/PartnerVideoRender.h \
    src/VideoRender/VideoRender.h \
    src/Network/NetworkReceiver.h \
    src/Network/NetworkSender.h \
    src/InfoNetwork/InfoNetwork.h \
    src/Codec/EncodeFrame.h \
    src/Codec/DecodeFrame.h \
    src/utils/utils.h \
    src/MainWindow.h

FORMS += \
    src/View/MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Info.plist
