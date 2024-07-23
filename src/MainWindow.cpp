#include "MainWindow.h"

#include "./Utils/Define.h"
#include "./VideoRender/CustomWidget/NV12Widget.h"
#include "./VideoRender/CustomWidget/Yuv420Widget.h"
#include "QStringList"
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

    (applicationPort == 8080) ? partnerPort = 8081 : partnerPort = 8080;

    infoTimer->start(1000);

    QGridLayout *gridLayout =
        qobject_cast<QGridLayout *>(ui->centralwidget->layout());

    _partnerRenderWidget = std::make_shared<Yuv420Widget>(this);
    _localRenderWidget = std::make_shared<NV12Widget>(this);

    if (gridLayout) {
        int row, column, rowSpan, columnSpan;
        gridLayout->getItemPosition(
            gridLayout->indexOf(ui->videoFrameLabelPartner), &row, &column,
            &rowSpan, &columnSpan);
        gridLayout->removeWidget(ui->videoFrameLabelPartner);
        delete ui->videoFrameLabelPartner;
        gridLayout->addWidget(
            dynamic_cast<QWidget *>(_partnerRenderWidget.get()), row, column,
            rowSpan, columnSpan);

        gridLayout->getItemPosition(
            gridLayout->indexOf(ui->videoFrameLabelLocal), &row, &column,
            &rowSpan, &columnSpan);
        gridLayout->removeWidget(ui->videoFrameLabelLocal);
        delete ui->videoFrameLabelLocal;
        gridLayout->addWidget(dynamic_cast<QWidget *>(_localRenderWidget.get()),
                              row, column, rowSpan, columnSpan);
        _callController->setVideoFrameLabelYUV420(_partnerRenderWidget);
        _callController->setVideoFrameLabelNV12(_localRenderWidget);
    } else
        qDebug() << "Error UI";

    setUpOption();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setUpOption() {
    QStringList codecOptions = {"H264", "AV1"};
    ui->comboBoxCodec->addItems(codecOptions);
    QStringList sizeOptions;
    sizeOptions << "240p" << "360p"
                << "480p"
                << "720p";
    ui->comboBoxSize->addItems(sizeOptions);
    connect(ui->comboBoxSize,
            QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
            [=](const QString &selectedSize) {
                ui->comboBoxBitrate->clear();
                if (selectedSize == "240p") {
                    QStringList bitrateOptions = {
                        "30K",  "40K",  "50K",  "60K",  "70K",  "80K",
                        "90K",  "100K", "150K", "200K", "300K", "400K",
                        "500K", "700K", "1M",   "1.2M", "1.5M", "2M",
                        "2.5M", "5M",   "10M",  "15M"};
                    ui->comboBoxBitrate->addItems(bitrateOptions);
                } else if (selectedSize == "360p") {
                    QStringList bitrateOptions = {
                        "100K", "150K", "200K", "300K", "400K", "500K",
                        "700K", "1M",   "1.2M", "1.5M", "1.7M", "2M"};
                    ui->comboBoxBitrate->addItems(bitrateOptions);
                } else if (selectedSize == "480p") {
                    QStringList bitrateOptions = {
                        "400K", "500K", "600K", "700K", "800K",
                        "1M",   "1.2M", "1.5M", "1.7M", "2M"};
                    ui->comboBoxBitrate->addItems(bitrateOptions);
                } else if (selectedSize == "720p") {
                    QStringList bitrateOptions = {"500K", "700K", "1M", "1.2M",
                                                  "1.5M", "1.7M", "2M", "2.5M"};
                    ui->comboBoxBitrate->addItems(bitrateOptions);
                }
            });
    if (codec == CODEC_H264) {
        ui->comboBoxSize->setCurrentText("720p");
        ui->comboBoxBitrate->setCurrentText("1.5M");
    } else {
        ui->comboBoxSize->setCurrentText("360p");
        ui->comboBoxBitrate->setCurrentText("150K");
    }
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
}

void MainWindow::onBtnStartCallClicked() {
    qDebug() << "onBtnStartCallClicked";
    QString selectedCodec = ui->comboBoxCodec->currentText();
    QString selectedSize = ui->comboBoxSize->currentText();
    QString selectedBitrate = ui->comboBoxBitrate->currentText();
    qDebug() << "Selected Size:" << selectedSize << selectedBitrate;

    // explain options
    int width, height;
    if (selectedSize == "240p") {
        width = 426;
        height = 240;
    } else if (selectedSize == "360p") {
        width = 640;
        height = 360;
    } else if (selectedSize == "480p") {
        width = 854;
        height = 480;
    } else if (selectedSize == "720p") {
        width = 1280;
        height = 720;
    } else {
        width = WIDTH;
        height = HEIGHT;
    }
    int bitrate = BITRATE;
    if (selectedBitrate.endsWith("K")) {
        bitrate =
            selectedBitrate.left(selectedBitrate.length() - 1).toInt() * 1000;
    } else if (selectedBitrate.endsWith("M")) {
        bitrate =
            selectedBitrate.left(selectedBitrate.length() - 1).toDouble() *
            1000000;
    }
    int codec = CODEC_H264;
    if (selectedCodec == "AV1")
        codec = CODEC_AV1;

    _callController->startCall("127.0.0.1", partnerPort, codec, width, height,
                               bitrate);
}

void MainWindow::onBtnStopCallClicked() {
    qDebug() << "onBtnStopCallClicked";
    _callController->stopCall();
}
