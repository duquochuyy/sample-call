QT       += core gui widgets multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/CallController/CallController.cpp \
    src/VideoCapture/QtVideoCapture.cpp \
    src/VideoCapture/VideoCapture.cpp \
    src/VideoCapture/QtVideoSurface.cpp \
    src/VideoRender/QtVideoRender.cpp \
    src/VideoRender/VideoRender.cpp \
    src/MainWindow.cpp

HEADERS += \
    src/CallController/CallController.h \
    src/VideoCapture/QtVideoCapture.h \
    src/VideoCapture/VideoCapture.h \
    src/VideoCapture/QtVideoSurface.h \
    src/VideoRender/QtVideoRender.h \
    src/VideoRender/VideoRender.h \
    src/MainWindow.h

FORMS += \
    src/View/MainWindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Info.plist
