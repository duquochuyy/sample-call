#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(std::shared_ptr<CallController> callController, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _callController(new CallController())
{
    ui->setupUi(this);

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onBtnStartCameraClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onBtnStopCameraClicked);

    _callController->setVideoFrameLabel(ui->videoFrameLabel);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onBtnStartCameraClicked()
{
    qDebug() << "onBtnStartCameraClicked";
    _callController->startCamera();
}

void MainWindow::onBtnStopCameraClicked()
{
    _callController->stopCamera();
}
