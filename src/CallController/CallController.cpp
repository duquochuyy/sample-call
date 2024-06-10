#include "./CallController.h"

CallController::CallController(int port)
{
    _vidCapture.reset(new QtVideoCapture());
    _vidCapture->registerCallback(this);
    _localRender.reset(new QtVideoRender());
    _localRender->registerCallback(this);
    _partnerRender.reset(new PartnerVideoRender());
    _partnerRender->registerCallback(this);

    _networkReceiver.reset(new NetworkReceiver(port));
    _networkReceiver->registerCallback(this);
    _networkReceiver->startListening();

    _networkSender.reset(new NetworkSender(port));
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
        _networkSender->sendData(frame);
}

void CallController::onReceiveFrame(const ZVideoFrame &frame)
{
    _partnerRender->render(frame);
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
