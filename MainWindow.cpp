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

    connect(cameraHandler, &CameraHandler::newFrameCaptured, this, &MainWindow::onNewFrameCaptured);

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onBtnStartCameraClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onBtnStopCameraClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadAndDisplayImage()
{
    if (QFile::exists(lastLoadedFrame)) {
        QImage image(lastLoadedFrame);
        if (!image.isNull()) {
            ui->videoFrameLabel->setPixmap(QPixmap::fromImage(image).scaled(
                ui->videoFrameLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            qDebug() << "Image loaded and displayed from" << lastLoadedFrame;
        }
    }
}

void MainWindow::onNewFrameCaptured()
{
    loadAndDisplayImage();
}

void MainWindow::onBtnStartCameraClicked()
{
    cameraHandler->startCamera();
}

void MainWindow::onBtnStopCameraClicked()
{
    cameraHandler->stopCamera();
}
