#include "./CallController.h"

CallController::CallController(int port)
    : applicationPort(port),
      _valueInfo(std::make_shared<ZValueInfo>()),
      connectedPartner(false) {
    _vidCapture.reset(new QtVideoCapture());
    _vidCapture->registerCallback(this);
    _vidCapture->setSetting(WIDTH, HEIGHT);

    _localRender.reset(new QtVideoRender());
    _localRender->registerCallback(this);
    _partnerRender.reset(new PartnerVideoRender());
    _partnerRender->registerCallback(this);

    _networkReceiver.reset(new NetworkReceiver(port));
    _networkReceiver->registerCallback(this);
    _networkReceiver->startListening();

    _networkSender.reset(new NetworkSender(port));
    _networkSender->registerCallback(this);

    _encodeFrame.reset(new EncodeFrame(WIDTH, HEIGHT));
    _encodeFrame->registerCallback(this);
    _decodeFrame.reset(new DecodeFrame(WIDTH, HEIGHT));
    _decodeFrame->registerCallback(this);

    _convert.reset(new Convert());

    _vidCapture->start();

    std::thread([this]() { processConvertRenderLocal(); }).detach();
}

CallController::~CallController() {
    _networkReceiver->disconnect();
    _networkSender->disconnect();
}

void CallController::onNewVideoFrameRawFormat(const ZRootFrame &frame) {
    auto framePtr = std::make_shared<ZRootFrame>(std::move(frame));
    localQueue.push(framePtr);
    if (connectedPartner)
        convertRawDataQueue.push(framePtr);
}

void CallController::onReceiveDataFrame(const std::vector<uchar> &fullFrameData,
                                        uint64_t timestamp) {
    auto framePtr = std::make_shared<ZEncodedFrame>(std::move(fullFrameData),
                                                    fullFrameData.size(), WIDTH,
                                                    HEIGHT, timestamp);
    decodeQueue.push(framePtr);
}

void CallController::processConvertRenderLocal() {
    while (true) {
        std::shared_ptr<ZRootFrame> framePtr;
        localQueue.waitPop(framePtr);

        if (localQueue.size() > 5) {
            localQueue.pop();
        }

        processLocalConvertTime.start();
        // convert -> render local
        // QImage imageLocal;
        // _convert->processNV12DatatToRGB(framePtr->nv12Data, framePtr->width,
        //                                 framePtr->height, imageLocal);
        // _localRender->render(imageLocal);

        _localRenderWidget->setFrameData(framePtr.get()->nv12Data,
                                         framePtr->width, framePtr->height);

        processLocalConvertTime.stop();
        double localConvertTime = processLocalConvertTime.getAverageTime();
        if (localConvertTime != -1) {
            _valueInfo->localConvertTime = localConvertTime;
        }
    }
}

void CallController::processConvertRawData() {
    while (connectedPartner) {
        std::shared_ptr<ZRootFrame> framePtr;
        convertRawDataQueue.waitPop(framePtr);

        if (localQueue.size() > 5) {
            localQueue.pop();
        }

        processEncodeConvertTime.start();
        // convert nv12 -> yuv420 frame to encode
        std::vector<uchar> yuv420pData;
        _convert->processNV12DatatToYUV420(framePtr.get()->nv12Data,
                                           framePtr.get()->width,
                                           framePtr.get()->height, yuv420pData);
        std::shared_ptr<ZVideoFrame> frame = std::make_shared<ZVideoFrame>(
            std::move(yuv420pData), framePtr->width, framePtr->height,
            framePtr->timestamp);
        processEncodeConvertTime.stop();
        double encodeConvertTime = processEncodeConvertTime.getAverageTime();
        if (encodeConvertTime != -1) {
            _valueInfo->encodeConvertTime = encodeConvertTime;
        }
        encodeQueue.push(frame);
    }
}

void CallController::processEncodeSend() {
    while (connectedPartner) {
        std::shared_ptr<ZVideoFrame> framePtr;
        encodeQueue.waitPop(framePtr);

        if (encodeQueue.size() > 5) {
            encodeQueue.pop();
        }

        processEncodeTime.start();
        // encode
        ZEncodedFrame encodedFrame;
        _encodeFrame->encodeYUV420ToH264(*framePtr, encodedFrame);
        processEncodeTime.stop();
        double encodeTime = processEncodeTime.getAverageTime();
        if (encodeTime != -1) {
            _valueInfo->encodeTime = encodeTime;
        }

        _networkSender->addNewEncodedFrame(encodedFrame);
    }
}

void CallController::processDecodeRender() {
    while (connectedPartner) {
        std::shared_ptr<ZEncodedFrame> framePtr;
        decodeQueue.waitPop(framePtr);

        if (decodeQueue.size() > 5) {
            decodeQueue.pop();
        }

        processDecodeTime.start();
        // decode -> yuv420
        std::shared_ptr<ZVideoFrame> decodedFrame =
            std::make_shared<ZVideoFrame>();
        _decodeFrame->decodeH264ToYUV420(framePtr.get()->encodedData,
                                         framePtr.get()->timestamp,
                                         decodedFrame);
        processDecodeTime.stop();
        double decodeTime = processDecodeTime.getAverageTime();
        if (decodeTime != -1) {
            _valueInfo->decodeTime = decodeTime;
        }
        convertPartnerQueue.push(decodedFrame);
    }
}

void CallController::processConvertPartnerThread() {
    while (connectedPartner) {
        std::shared_ptr<ZVideoFrame> framePtr;
        convertPartnerQueue.waitPop(framePtr);

        if (convertPartnerQueue.size() > 5) {
            convertPartnerQueue.pop();
        }

        processPartnerConvertTime.start();

        // convert to render partner
        // QImage imagePartner;
        // _convert->convertYUV420ToRGB(framePtr.get()->yuv420pData,
        // framePtr.get()->width, framePtr.get()->height, imagePartner);
        // _partnerRender->render(imagePartner);

        // test render yuv gpu
        _partnerRenderWidget->setFrameData(framePtr.get()->yuv420pData,
                                           framePtr->width, framePtr->height);

        processPartnerConvertTime.stop();
        double partnerConvertTime = processPartnerConvertTime.getAverageTime();
        if (partnerConvertTime != -1) {
            _valueInfo->partConvertTime = partnerConvertTime;
        }
    }
}

void CallController::onAcceptedConnection(std::string partnerIP,
                                          int partnerPort) {
    qDebug() << "Connection accepted. Partner port: " << partnerPort;
    if (!connectedPartner) {
        startCall(partnerIP, partnerPort);
    }
}

void CallController::onRequestDisconnect() { stopCall(); }

void CallController::setVideoFrameLabelLocal(QLabel *&label) {
    _localRender->setVideoFrameLabel(label);
}

void CallController::setVideoFrameLabelPartner(QLabel *&label) {
    _partnerRender->setVideoFrameLabel(label);
}

void CallController::setVideoFrameLabelYUV420(
    std::shared_ptr<Yuv420Widget> label) {
    _partnerRenderWidget = label;
}

void CallController::setVideoFrameLabelNV12(std::shared_ptr<NV12Widget> label) {
    _localRenderWidget = label;
}

void CallController::startCall(std::string partnerIP, int partnerPort) {
    if (!connectedPartner) {
        qDebug() << "startCall to port" << partnerPort;
        bool connected =
            _networkSender->handleConnectPartner(partnerIP, partnerPort);
        if (!connected) {
            qDebug() << "Failed to connect to" << partnerPort;
        } else {
            qDebug() << "Connected to" << partnerPort;
            connectedPartner = true;
            std::thread([this]() { processConvertRawData(); }).detach();
            std::thread([this]() { processEncodeSend(); }).detach();
            std::thread([this]() { processDecodeRender(); }).detach();
            std::thread([this]() { processConvertPartnerThread(); }).detach();
        }
    }
}

void CallController::stopCall() {
    _vidCapture->stop();
    _networkSender->disconnect();
    // _networkReceiver->disconnect();
    connectedPartner = false;
}

std::shared_ptr<ZLabelRender> CallController::getLabelRender() const {
    return _valueInfo;
}

// render
void CallController::onShowInfoCapture(int width, int height, int fps) {
    _valueInfo->captureWidth = width;
    _valueInfo->captureHeight = height;
    _valueInfo->captureFps = fps;
}

void CallController::onShowInfoLocalFps(int fps) { _valueInfo->localFps = fps; }

void CallController::onShowInfoEncode(int width, int height, int fps) {
    _valueInfo->encodeWidth = width;
    _valueInfo->encodeHeight = height;
    _valueInfo->encodeFps = fps;
}

void CallController::onShowInfoSend(int fps, int pps, double bitrate) {
    _valueInfo->sendFps = fps;
    _valueInfo->sendPps = pps;
    _valueInfo->sendBitrate = bitrate;
}

void CallController::onShowInfoReceive(int fps, int pps, double bitrate) {
    _valueInfo->receiveFps = fps;
    _valueInfo->receivePps = pps;
    _valueInfo->receiveBitrate = bitrate;
}

void CallController::onShowInfoDecode(int width, int height, int fps) {
    _valueInfo->decodeWidth = width;
    _valueInfo->decodeHeight = height;
    _valueInfo->decodeFps = fps;
}

void CallController::onShowInfoPartnerFps(int fps) {
    _valueInfo->partnerFps = fps;
}
