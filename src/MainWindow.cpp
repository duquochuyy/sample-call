#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(std::shared_ptr<CallController> callController, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _callController(callController)
{
    ui->setupUi(this);
    ui->videoFrameLabelLocal->raise();

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onBtnStartCallClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onBtnStopCallClicked);

    _callController->setVideoFrameLabelLocal(ui->videoFrameLabelLocal);
    _callController->setVideoFrameLabelPartner(ui->videoFrameLabelPartner);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onBtnStartCallClicked()
{
    qDebug() << "onBtnStartCallClicked";
    QString ipText = ui->textIP->toPlainText();
    QString portText = ui->textPort->toPlainText();
    int partnerPort = portText.toInt();
    // std::string partnerIP = ipText.toStdString();
    std::string partnerIP = "127.0.0.1";
    _callController->startCall(partnerIP, partnerPort);
}

void MainWindow::onBtnStopCallClicked()
{
    qDebug() << "onBtnStopCallClicked";
    _callController->stopCall();
}
