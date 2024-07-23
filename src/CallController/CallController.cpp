#include "./CallController.h"

#include "./../Codec/DecodeAV1.h"
#include "./../Codec/DecodeH264.h"
#include "./../Codec/EncodeAV1.h"
#include "./../Codec/EncodeH264.h"
#include "./../VideoCapture/QtVideoCapture.h"
#include "./../VideoRender/PartnerVideoRender.h"
#include "./../VideoRender/QtVideoRender.h"

CallController::CallController(int port)
    : applicationPort(port),
      _valueInfo(std::make_shared<ZValueInfo>()),
      connectedPartner(false) {
    _vidCapture.reset(new QtVideoCapture());
    _vidCapture->registerCallback(this);
    _vidCapture->setSetting(CAPTURE_WIDTH, CAPTURE_HEIGHT);

    _localRender.reset(new QtVideoRender());
    _localRender->registerCallback(this);
    _partnerRender.reset(new PartnerVideoRender());
    _partnerRender->registerCallback(this);

    _networkReceiver.reset(new NetworkReceiver(port));
    _networkReceiver->registerCallback(this);
    _networkReceiver->startListening();

    _networkSender.reset(new NetworkSender(port));
    _networkSender->registerCallback(this);

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
    if (connectedPartner) {
        auto framePtr = std::make_shared<ZEncodedFrame>(
            std::move(fullFrameData), fullFrameData.size(), WIDTH, HEIGHT,
            timestamp);
        decodeQueue.push(framePtr);
    }
}

void CallController::processConvertRenderLocal() {
    while (true) {
        std::shared_ptr<ZRootFrame> framePtr;
        localQueue.waitPop(framePtr);

        if (localQueue.size() > 5) {
            localQueue.pop();
        }

        processLocalConvertTime.start();

        _localRender->render(framePtr, ImageFormat::NV12);

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

        // resize
        std::vector<uchar> outputNv12Data;
        int outputWidth = this->width;
        int outputHeight = this->height;
        _convert->resizeNV12DataCapture(
            framePtr.get()->nv12Data, framePtr.get()->width,
            framePtr.get()->height, outputNv12Data, outputWidth, outputHeight);

        // convert nv12 to yuv420
        std::vector<uchar> yuv420pData;
        _convert->convertNV12DatatToYUV420(outputNv12Data, outputWidth,
                                           outputHeight, yuv420pData);

        std::shared_ptr<ZVideoFrame> frame =
            std::make_shared<ZVideoFrame>(std::move(yuv420pData), outputWidth,
                                          outputHeight, framePtr->timestamp);

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
        _encodeFrame->encode(*framePtr, encodedFrame);
        if (codec == CODEC_AV1)
            processEncodeTime.distanceTime(encodedFrame.timestamp,
                                           framePtr->timestamp);
        else
            processEncodeTime.stop();
        double encodeTime = processEncodeTime.getAverageTime();
        if (encodeTime != -1) {
            _valueInfo->encodeTime = encodeTime;
        }

        // test decode
        // processDecodeTime.start();
        // // // decode -> yuv420
        // std::shared_ptr<ZVideoFrame> decodedFrame =
        //     std::make_shared<ZVideoFrame>();
        // _decodeFrame->decode(encodedFrame.encodedData,
        // encodedFrame.timestamp,
        //                      decodedFrame);
        // processDecodeTime.stop();
        // double decodeTime = processDecodeTime.getAverageTime();
        // if (decodeTime != -1) {
        //     _valueInfo->decodeTime = decodeTime;
        // }
        // convertPartnerQueue.push(decodedFrame);

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
        _decodeFrame->decode(framePtr.get()->encodedData,
                             framePtr.get()->timestamp, decodedFrame);
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

        _partnerRender->render(framePtr, ImageFormat::YUV420);

        processPartnerConvertTime.stop();
        double partnerConvertTime = processPartnerConvertTime.getAverageTime();
        if (partnerConvertTime != -1) {
            _valueInfo->partConvertTime = partnerConvertTime;
        }
    }
}

void CallController::onAcceptedConnection(std::string partnerIP,
                                          int partnerPort, int codec, int width,
                                          int height, int bitrate) {
    qDebug() << "Connection accepted. Partner port: " << partnerPort
             << "with option: " << codec << width << height << bitrate;
    if (!connectedPartner) {
        this->codec = codec;
        this->width = width;
        this->height = height;
        this->bitrate = bitrate;
        startCall(partnerIP, partnerPort, codec, width, height, bitrate);
    }
}

void CallController::onRequestDisconnect() { stopCall(); }

void CallController::setVideoFrameLabelYUV420(
    std::shared_ptr<YuvWidget> &widget) {
    _partnerRender->setVideoFrameLabel(widget);
}

void CallController::setVideoFrameLabelNV12(
    std::shared_ptr<YuvWidget> &widget) {
    _localRender->setVideoFrameLabel(widget);
}

void CallController::startCall(std::string partnerIP, int partnerPort,
                               int codec, int width, int height, int bitrate) {
    this->codec = codec;
    this->width = width;
    this->height = height;
    this->bitrate = bitrate;
    qDebug() << width << height << bitrate;
    stopCall();
    if (!connectedPartner) {
        qDebug() << "startCall to port" << partnerPort;
        qDebug() << "set up codec" << partnerPort;

        if (codec == CODEC_AV1) {
            qDebug() << "use av1";
            _encodeFrame.reset(new EncodeAV1(width, height, bitrate));
            _decodeFrame.reset(new DecodeAV1(width, height));
        } else {
            qDebug() << "use h264";
            _encodeFrame.reset(new EncodeH264(width, height, bitrate));
            _decodeFrame.reset(new DecodeH264(width, height));
        }
        qDebug() << "start register callback";
        _encodeFrame->registerCallback(this);
        _decodeFrame->registerCallback(this);

        qDebug() << "start connect";
        bool connected = false;
        connected =
            // true;
            _networkSender->handleConnectPartner(partnerIP, partnerPort, codec,
                                                 width, height, bitrate);

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
    if (connectedPartner) {
        _networkSender->disconnect();
        _networkReceiver->disconnect();
        connectedPartner = false;
        localQueue.clear();
        convertRawDataQueue.clear();
        encodeQueue.clear();
        decodeQueue.clear();
        convertPartnerQueue.clear();
        _networkReceiver->startListening();
    }
}

std::shared_ptr<ZValueInfo> CallController::getLabelRender() const {
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
