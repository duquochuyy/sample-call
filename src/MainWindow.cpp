#include "MainWindow.h"

#include "ui_MainWindow.h"

MainWindow::MainWindow(std::unique_ptr<CallController> callController,
                       int applicationPort, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _callController(std::move(callController)),
      applicationPort(applicationPort),
      infoTimer(new QTimer(this)) {
    ui->setupUi(this);
    ui->videoFrameLabelLocal->raise();

    connect(ui->startButton, &QPushButton::clicked, this,
            &MainWindow::onBtnStartCallClicked);
    connect(ui->stopButton, &QPushButton::clicked, this,
            &MainWindow::onBtnStopCallClicked);
    connect(infoTimer, &QTimer::timeout, this, &MainWindow::renderInfo);

    // for render camera
    _callController->setVideoFrameLabelLocal(ui->videoFrameLabelLocal);
    _callController->setVideoFrameLabelPartner(ui->videoFrameLabelPartner);

    (applicationPort == 8080) ? partnerPort = 8081 : partnerPort = 8080;

    infoTimer->start(1000);

    // test render yuv gpu
    // Remove the old QLabel from the layout
    _partnerRenderWidget = std::make_shared<Yuv420Widget>(this);
    QGridLayout *gridLayout =
        qobject_cast<QGridLayout *>(ui->centralwidget->layout());
    if (gridLayout) {
        int row, column, rowSpan, columnSpan;
        gridLayout->getItemPosition(
            gridLayout->indexOf(ui->videoFrameLabelPartner), &row, &column,
            &rowSpan, &columnSpan);
        gridLayout->removeWidget(ui->videoFrameLabelPartner);
        delete ui->videoFrameLabelPartner;
        gridLayout->addWidget(_partnerRenderWidget.get(), row, column, rowSpan,
                              columnSpan);
    }
    qDebug() << "YuvWidget initial size:" << _partnerRenderWidget->size();
    _callController->setVideoFrameLabelYUV420(_partnerRenderWidget);

    _localRenderWidget = std::make_shared<NV12Widget>(this);
    if (gridLayout) {
        int row, column, rowSpan, columnSpan;
        gridLayout->getItemPosition(
            gridLayout->indexOf(ui->videoFrameLabelLocal), &row, &column,
            &rowSpan, &columnSpan);
        gridLayout->removeWidget(ui->videoFrameLabelLocal);
        delete ui->videoFrameLabelLocal;
        gridLayout->addWidget(_localRenderWidget.get(), row, column, rowSpan,
                              columnSpan);
    }
    qDebug() << "NV12Widget initial size:" << _localRenderWidget->size();
    _callController->setVideoFrameLabelNV12(_localRenderWidget);
}

MainWindow::~MainWindow() {
    delete ui;
    // delete videoDisplay;
}

void MainWindow::renderInfo() {
    auto labelRender = _callController->getLabelRender();
    // Capture
    ui->labelCaptureWidth->setText(QString::number(labelRender->captureWidth));
    ui->labelCaptureHeight->setText(
        QString::number(labelRender->captureHeight));
    ui->labelCaptureFps->setText(QString::number(labelRender->captureFps));
    ui->labelLocalConvertTime->setText(
        QString::number(labelRender->localConvertTime));
    ui->labelLocalFps->setText(QString::number(labelRender->localFps));

    // Encode
    ui->labelEncodeWidth->setText(QString::number(labelRender->encodeWidth));
    ui->labelEncodeHeight->setText(QString::number(labelRender->encodeHeight));
    ui->labelEncodeFps->setText(QString::number(labelRender->encodeFps));
    ui->labelEncodeConvertTime->setText(
        QString::number(labelRender->encodeConvertTime));
    ui->labelEncodeTime->setText(QString::number(labelRender->encodeTime));

    // Send
    ui->labelSendFps->setText(QString::number(labelRender->sendFps));
    ui->labelSendPps->setText(QString::number(labelRender->sendPps));
    ui->labelSendBitrate->setText(QString::number(labelRender->sendBitrate));

    // Receive
    ui->labelReceiveFps->setText(QString::number(labelRender->receiveFps));
    ui->labelReceivePps->setText(QString::number(labelRender->receivePps));
    ui->labelReceiveBitrate->setText(
        QString::number(labelRender->receiveBitrate));

    // Decode
    ui->labelDecodeWidth->setText(QString::number(labelRender->decodeWidth));
    ui->labelDecodeHeight->setText(QString::number(labelRender->decodeHeight));
    ui->labelDecodeFps->setText(QString::number(labelRender->decodeFps));
    ui->labelDecodeTime->setText(QString::number(labelRender->decodeTime));

    // Partner render
    ui->labelPartnerFps->setText(QString::number(labelRender->partnerFps));
    ui->labelPartnerConvertTime->setText(
        QString::number(labelRender->partConvertTime));
    // qDebug() << "time convert + encode" << labelRender->encodeConvertTime +
    // labelRender->encodeTime;
}

void MainWindow::onBtnStartCallClicked() {
    qDebug() << "onBtnStartCallClicked";
    _callController->startCall("127.0.0.1", partnerPort);
}

void MainWindow::onBtnStopCallClicked() {
    qDebug() << "onBtnStopCallClicked";
    _callController->stopCall();
}
