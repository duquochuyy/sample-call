#ifndef CALLCONTROLLER_H
#define CALLCONTROLLER_H

#include <QByteArray>
#include <QLabel>
#include <memory>
#include <thread>
#include <vector>

#include "./../Codec/DecodeFrame.h"
#include "./../Codec/EncodeFrame.h"
#include "./../Common/ZEncodedFrame.h"
#include "./../Common/ZRootFrame.h"
#include "./../Common/ZValueInfo.h"
#include "./../Common/ZVideoFrame.h"
#include "./../Network/NetworkReceiver.h"
#include "./../Network/NetworkSender.h"
#include "./../Utils/Convert.h"
#include "./../Utils/Define.h"
#include "./../Utils/NV12Widget.h"
#include "./../Utils/ThreadSafeQueue.h"
#include "./../Utils/TimeTracker.h"
#include "./../Utils/Yuv420Widget.h"
#include "./../VideoCapture/QtVideoCapture.h"
#include "./../VideoCapture/QtVideoSurface.h"
#include "./../VideoCapture/VideoCapture.h"
#include "./../VideoRender/PartnerVideoRender.h"
#include "./../VideoRender/QtVideoRender.h"
#include "./../VideoRender/VideoRender.h"

class CallController : public VideoCapture::Callback,
                       public VideoRender::Callback,
                       public NetworkReceiver::Callback,
                       public NetworkSender::Callback,
                       public EncodeFrame::Callback,
                       public DecodeFrame::Callback {
   public:
    CallController(int port = 8080);
    ~CallController();
    // for capture
    void onNewVideoFrameRawFormat(const ZRootFrame &frame) override;

    // for render
    void setVideoFrameLabelLocal(QLabel *&label);
    void setVideoFrameLabelPartner(QLabel *&label);
    void setVideoFrameLabelYUV420(std::shared_ptr<Yuv420Widget> label);
    void setVideoFrameLabelNV12(std::shared_ptr<NV12Widget> label);

    // for receive
    void onReceiveDataFrame(const std::vector<uchar> &fullFrameData,
                            uint64_t timestamp) override;
    void onAcceptedConnection(std::string partnerIP, int partnerPort) override;
    void onRequestDisconnect() override;

    // for send

    // for log info
    void onShowInfoCapture(int width, int height, int fps) override;  // capture
    void onShowInfoLocalFps(int fps) override;                        // local
    void onShowInfoEncode(int width, int height, int fps) override;   // encode
    void onShowInfoSend(int fps, int pps, double bitrate) override;   // send
    void onShowInfoReceive(int fps, int pps, double bitrate) override;  // send
    void onShowInfoDecode(int width, int height, int fps) override;  // decode
    void onShowInfoPartnerFps(int fps) override;  // partner fps

   public:
    void startCall(std::string partnerIP, int partnerPort);
    void stopCall();
    std::shared_ptr<ZValueInfo> getLabelRender() const;

   private:
    void processConvertRenderLocal();
    void processConvertRawData();
    void processEncodeSend();
    void processDecodeRender();
    void processConvertPartnerThread();

   private:
    std::shared_ptr<VideoCapture> _vidCapture;
    std::shared_ptr<VideoRender> _localRender;
    std::shared_ptr<VideoRender> _partnerRender;
    std::shared_ptr<NetworkSender> _networkSender;
    std::shared_ptr<NetworkReceiver> _networkReceiver;
    std::shared_ptr<EncodeFrame> _encodeFrame;
    std::shared_ptr<DecodeFrame> _decodeFrame;
    std::shared_ptr<Convert> _convert;
    // is calling to partner
    std::atomic<bool> connectedPartner;
    int applicationPort;
    // for render ui
    std::shared_ptr<ZValueInfo> _valueInfo;
    // for tracker time process
    TimeTracker processLocalConvertTime;
    TimeTracker processEncodeConvertTime;
    TimeTracker processEncodeTime;
    TimeTracker processDecodeConvertTime;
    TimeTracker processDecodeTime;
    TimeTracker processPartnerConvertTime;
    // for queue frame
    ThreadSafeQueue<std::shared_ptr<ZRootFrame>>
        localQueue;  // for convert local NV12 -> RGB
    ThreadSafeQueue<std::shared_ptr<ZRootFrame>>
        convertRawDataQueue;  // for convert to send NV12 -> YUV420
    ThreadSafeQueue<std::shared_ptr<ZVideoFrame>>
        encodeQueue;  // for encode to send
    ThreadSafeQueue<std::shared_ptr<ZEncodedFrame>>
        decodeQueue;  // for decode partner
    ThreadSafeQueue<std::shared_ptr<ZVideoFrame>>
        convertPartnerQueue;  // for convert to render partner
    // test render yuv gpu
    std::shared_ptr<Yuv420Widget> _partnerRenderWidget;
    std::shared_ptr<NV12Widget> _localRenderWidget;
};

#endif
