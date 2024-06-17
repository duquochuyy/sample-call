#include "./CallController.h"

CallController::CallController(int port) : applicationPort(port)
{
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

    _infoNetworkSender.reset(new InfoNetwork());
    _infoNetworkReceiver.reset(new InfoNetwork());

    _encodeFrame.reset(new EncodeFrame(WIDTH, HEIGHT));
    _decodeFrame.reset(new DecodeFrame(WIDTH, HEIGHT));

    _vidCapture->start();
}

CallController::~CallController()
{
    _networkReceiver->disconnect();
    _networkSender->disconnect();
}

void CallController::onNewVideoFrame(const ZVideoFrame &frame)
{
    _localRender->render(frame);
    if (connectedPartner)
    {
        const ZEncodedFrame encodedFrame = _encodeFrame->encodeYUV420ToH264(frame);
        _networkSender->addNewEncodedFrame(encodedFrame);
    }
}

void CallController::onReceiveFrame(const std::vector<uint8_t> &encodedData, const uint64_t timestamp)
{
    const ZVideoFrame decodedFrame = _decodeFrame->decodeH264ToYUV420(encodedData, timestamp);
    _partnerRender->render(decodedFrame);
}

void CallController::onAcceptedConnection(std::string partnerIP, int partnerPort)
{
    qDebug() << "Connection accepted. Partner port: " << partnerPort;
    if (!connectedPartner)
    {
        startCall(partnerIP, partnerPort);
    }
}

void CallController::onRequestDisconnect()
{
    stopCall();
}

void CallController::onStateChanged()
{
}

void CallController::setVideoFrameLabelLocal(QLabel *label)
{
    _localRender->setVideoFrameLabel(label);
}

void CallController::setVideoFrameLabelPartner(QLabel *label)
{
    _partnerRender->setVideoFrameLabel(label);
}

void CallController::setLabelInfoSend(QLabel *width, QLabel *height, QLabel *fps, QLabel *bitrate)
{
    _infoNetworkSender->setLabel(width, height, fps, bitrate);
}

void CallController::setLabelInfoReceive(QLabel *width, QLabel *height, QLabel *fps, QLabel *bitrate)
{
    _infoNetworkReceiver->setLabel(width, height, fps, bitrate);
}

void CallController::startCall(std::string partnerIP, int partnerPort)
{
    if (!connectedPartner)
    {
        qDebug() << "startCall to port" << partnerPort;
        bool connected = _networkSender->handleConnectPartner(partnerIP, partnerPort);
        if (!connected)
        {
            qDebug() << "Failed to connect to" << partnerPort;
        }
        else
        {
            qDebug() << "Connected to" << partnerPort;
            connectedPartner = true;
        }
    }
}

void CallController::stopCall()
{
    _vidCapture->stop();
    _networkSender->disconnect();
    // _networkReceiver->disconnect();
    connectedPartner = false;
}

void CallController::onRenderInfoReceiver(int width, int height, int fps, double bitrate)
{
    _infoNetworkReceiver->renderInfo(width, height, fps, bitrate);
}

void CallController::onRenderInfoSender(int width, int height, int fps, double bitrate)
{
    _infoNetworkSender->renderInfo(width, height, fps, bitrate);
}
