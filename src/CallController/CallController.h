#ifndef CALLCONTROLLER_H
#define CALLCONTROLLER_H

#include <memory>
#include <QLabel>
#include <vector>

#include "./../VideoCapture/QtVideoCapture.h"
#include "./../VideoCapture/VideoCapture.h"
#include "./../VideoRender/VideoRender.h"
#include "./../VideoRender/QtVideoRender.h"
#include "./../VideoRender/PartnerVideoRender.h"
#include "./../Network/NetworkReceiver.h"
#include "./../Network/NetworkSender.h"

class CallController : public VideoCapture::Callback,
                       public VideoRender::Callback,
                       public NetworkReceiver::Callback
{
public:
    CallController(int port = 8080);
    //    CallController(std::shared_ptr<CallController> callController) {};
    ~CallController();
    // for capture
    void onNewVideoFrame(const ZVideoFrame &frame) override;

    // for render
    void onStateChanged() override;
    void setVideoFrameLabelLocal(QLabel *label);
    void setVideoFrameLabelPartner(QLabel *label);

    void setLabelInfoSend(QLabel *width, QLabel *height, QLabel *FPS, QLabel *bitrate);
    void setLabelInfoReceive(QLabel *width, QLabel *height, QLabel *FPS, QLabel *bitrate);

    // for receive
    void onReceiveFrame(const ZVideoFrame &frame) override;
    void onAcceptedConnection(std::string partnerIP, int partnerPort) override;
    void onRequestDisconnect() override;

public:
    void startCall(std::string partnerIP, int partnerPort);
    void stopCall();

    void connectToPartner() {}

private:
    std::shared_ptr<VideoCapture> _vidCapture;
    std::shared_ptr<VideoRender> _localRender;
    std::shared_ptr<VideoRender> _partnerRender;
    std::shared_ptr<NetworkSender> _networkSender;
    std::shared_ptr<NetworkReceiver> _networkReceiver;
    // is calling to partner
    bool connectedPartner = false;
    int applicationPort;
};

#endif
