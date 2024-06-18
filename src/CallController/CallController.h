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
#include "./../InfoNetwork/InfoNetwork.h"
#include "./../Codec/EncodeFrame.h"
#include "./../Codec/DecodeFrame.h"

#define WIDTH 1280
#define HEIGHT 720
#define FPS 30

class CallController : public VideoCapture::Callback,
                       public VideoRender::Callback,
                       public NetworkReceiver::Callback,
                       public NetworkSender::Callback
{
public:
    CallController(int port = 8080);
    ~CallController();
    // for capture
    void onNewVideoFrame(const std::shared_ptr<ZVideoFrame> &frame) override;

    // for render
    void onStateChanged() override;
    void setVideoFrameLabelLocal(QLabel *label);
    void setVideoFrameLabelPartner(QLabel *label);

    void setLabelInfoSend(QLabel *width, QLabel *height, QLabel *fps, QLabel *bitrate);
    void setLabelInfoReceive(QLabel *width, QLabel *height, QLabel *fps, QLabel *bitrate);

    // for receive
    void onReceiveFrame(const std::vector<uchar> &encodedData, const uint64_t timestamp) override;
    void onAcceptedConnection(std::string partnerIP, int partnerPort) override;
    void onRequestDisconnect() override;
    void onRenderInfoReceiver(int width = WIDTH, int height = HEIGHT, int fps = FPS, double bitrate = 1.0) override;

    // for send
    void onRenderInfoSender(int width = WIDTH, int height = HEIGHT, int fps = FPS, double bitrate = 1.0) override;

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
    std::shared_ptr<InfoNetwork> _infoNetworkSender;
    std::shared_ptr<InfoNetwork> _infoNetworkReceiver;
    std::shared_ptr<EncodeFrame> _encodeFrame;
    std::shared_ptr<DecodeFrame> _decodeFrame;
    // is calling to partner
    bool connectedPartner = false;
    int applicationPort;
    // for thread encode and decode
    std::mutex _decodeMutex;
    std::mutex _encodeMutex;
};

#endif
