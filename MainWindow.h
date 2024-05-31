#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QString>
#include "CameraHandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // handle show video from frame
    void onNewFrameCaptured(const QImage &frame);

    // handle button
    void onBtnStartCameraClicked();
    void onBtnStopCameraClicked();
private:
    Ui::MainWindow *ui;
    CameraHandler *cameraHandler;
    QString lastLoadedFrame;
};

#endif // MAINWINDOW_H
