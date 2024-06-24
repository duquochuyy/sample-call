#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(std::unique_ptr<CallController> callController, int applicationPort, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _callController(std::move(callController)),
      applicationPort(applicationPort),
      infoTimer(new QTimer(this))
{
    ui->setupUi(this);
    ui->videoFrameLabelLocal->raise();

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onBtnStartCallClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onBtnStopCallClicked);
    connect(infoTimer, &QTimer::timeout, this, &MainWindow::renderInfo);

    // for render camera
    _callController->setVideoFrameLabelLocal(ui->videoFrameLabelLocal);
    _callController->setVideoFrameLabelPartner(ui->videoFrameLabelPartner);

    (applicationPort == 8080)
        ? partnerPort = 8081
        : partnerPort = 8080;

    infoTimer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::renderInfo()
{
    auto labelRender = _callController->getLabelRender();
    // Capture
    ui->labelCaptureWidth->setText(QString::number(labelRender->captureWidth));
    ui->labelCaptureHeight->setText(QString::number(labelRender->captureHeight));
    ui->labelCaptureFps->setText(QString::number(labelRender->captureFps));
    ui->labelLocalConvertTime->setText(QString::number(labelRender->localConvertTime));
    ui->labelLocalFps->setText(QString::number(labelRender->localFps));

    // Encode
    ui->labelEncodeWidth->setText(QString::number(labelRender->encodeWidth));
    ui->labelEncodeHeight->setText(QString::number(labelRender->encodeHeight));
    ui->labelEncodeFps->setText(QString::number(labelRender->encodeFps));
    ui->labelEncodeConvertTime->setText(QString::number(labelRender->encodeConvertTime));
    ui->labelEncodeTime->setText(QString::number(labelRender->encodeTime));
    ui->labelEncodeScaleTime->setText(QString::number(labelRender->encodeScaleTime));

    // Send
    ui->labelSendFps->setText(QString::number(labelRender->sendFps));
    ui->labelSendPps->setText(QString::number(labelRender->sendPps));
    ui->labelSendBitrate->setText(QString::number(labelRender->sendBitrate));

    // Receive
    ui->labelReceiveFps->setText(QString::number(labelRender->receiveFps));
    ui->labelReceivePps->setText(QString::number(labelRender->receivePps));
    ui->labelReceiveBitrate->setText(QString::number(labelRender->receiveBitrate));

    // Decode
    ui->labelDecodeWidth->setText(QString::number(labelRender->decodeWidth));
    ui->labelDecodeHeight->setText(QString::number(labelRender->decodeHeight));
    ui->labelDecodeFps->setText(QString::number(labelRender->decodeFps));
    ui->labelDecodeConvertTime->setText(QString::number(labelRender->decodeConvertTime));
    ui->labelDecodeTime->setText(QString::number(labelRender->decodeTime));
    ui->labelDecodeScaleTime->setText(QString::number(labelRender->decodeScaleTime));

    // Partner render
    ui->labelPartnerFps->setText(QString::number(labelRender->partnerFps));
    ui->labelPartnerConvertTime->setText(QString::number(labelRender->partConvertTime));
    // qDebug() << "time convert + encode" << labelRender->encodeConvertTime + labelRender->encodeTime;
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
