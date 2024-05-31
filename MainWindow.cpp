#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cameraHandler(new CameraHandler(this))
{
    ui->setupUi(this);

    QString documentsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    lastLoadedFrame = documentsPath + "/current_frame.jpg";

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onBtnStartCameraClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onBtnStopCameraClicked);

    connect(cameraHandler, &CameraHandler::newFrameCaptured, this, &MainWindow::onNewFrameCaptured);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewFrameCaptured(const QImage &frame)
{
    qDebug() << frame.format();
        ui->videoFrameLabel->setPixmap(QPixmap::fromImage(frame).scaled(
            ui->videoFrameLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::onBtnStartCameraClicked()
{
    cameraHandler->startCamera();
}

void MainWindow::onBtnStopCameraClicked()
{
    cameraHandler->stopCamera();
}
