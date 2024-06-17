#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(std::shared_ptr<CallController> callController, int applicationPort, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _callController(callController),
      applicationPort(applicationPort)
{
    ui->setupUi(this);
    ui->videoFrameLabelLocal->raise();

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onBtnStartCallClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onBtnStopCallClicked);

    _callController->setVideoFrameLabelLocal(ui->videoFrameLabelLocal);
    _callController->setVideoFrameLabelPartner(ui->videoFrameLabelPartner);
    _callController->setLabelInfoSend(ui->labelSendWidth, ui->labelSendHeight, ui->labelSendFps, ui->labelSendBitrate);
    _callController->setLabelInfoReceive(ui->labelReceiveWidth, ui->labelReceiveHeight, ui->labelReceiveFps, ui->labelReceiveBitrate);

    (applicationPort == 8080) ? partnerPort = 8081 : partnerPort = 8080;
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
    // int partnerPort = portText.toInt();
    //    int partnerPort = portText.toInt() - 1;
    //     std::string partnerIP = ipText.toStdString();
    _callController->startCall("127.0.0.1", partnerPort);
}

void MainWindow::onBtnStopCallClicked()
{
    qDebug() << "onBtnStopCallClicked";
    _callController->stopCall();
}
