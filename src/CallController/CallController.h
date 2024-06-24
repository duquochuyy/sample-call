#ifndef CALLCONTROLLER_H
#define CALLCONTROLLER_H

#include <memory>
#include <QLabel>
#include <vector>
#include <thread>

#include "./../VideoCapture/QtVideoCapture.h"
#include "./../VideoCapture/QtVideoSurface.h"
#include "./../VideoCapture/VideoCapture.h"
#include "./../VideoRender/VideoRender.h"
#include "./../VideoRender/QtVideoRender.h"
#include "./../VideoRender/PartnerVideoRender.h"
#include "./../Network/NetworkReceiver.h"
#include "./../Network/NetworkSender.h"
#include "./../Codec/EncodeFrame.h"
#include "./../Codec/DecodeFrame.h"
#include "./../utils/utils.h"
#include "./../utils/TimeTracker.h"
#include "./../utils/ThreadSafeQueue.h"

#include "./../Struct/ZRootFrame.h"
#include "./../Struct/ZEncodedFrame.h"
#include "./../Struct/ZVideoFrame.h"
#include "./../Struct/ZLabelRender.h"

#define WIDTH 1280
#define HEIGHT 720
#define FPS 30

class CallController : public VideoCapture::Callback,
                       public VideoRender::Callback,
                       public NetworkReceiver::Callback,
                       public NetworkSender::Callback,
                       public EncodeFrame::Callback,
                       public DecodeFrame::Callback
{
public:
    CallController(int port = 8080);
    ~CallController();
    // for capture
    void onNewVideoFrameRawFormat(const ZRootFrame &frame) override;

    // for render
    void onStateChanged() override;
    void setVideoFrameLabelLocal(QLabel *&label);
    void setVideoFrameLabelPartner(QLabel *&label);

    // for receive
    void onReceiveFrame(ZEncodedFrame &encodedFrame) override;
    void onReceiveDataFrame(const std::vector<uchar> &fullFrameData, uint64_t timestamp) override;
    void onAcceptedConnection(std::string partnerIP, int partnerPort) override;
    void onRequestDisconnect() override;

    // for send

    // for log info
    void onShowInfoCapture(int width, int height, int fps) override;   // capture
    void onShowInfoLocalFps(int fps) override;                         // local
    void onShowInfoEncode(int width, int height, int fps) override;    // encode
    void onShowInfoSend(int fps, int pps, double bitrate) override;    // send
    void onShowInfoReceive(int fps, int pps, double bitrate) override; // send
    void onShowInfoDecode(int width, int height, int fps) override;    // decode
    void onShowInfoPartnerFps(int fps) override;                       // partner fps

public:
    void startCall(std::string partnerIP, int partnerPort);
    void stopCall();
    std::shared_ptr<ZLabelRender> getLabelRender() const;

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
    // is calling to partner
    std::atomic<bool> connectedPartner;
    int applicationPort;
    // for thread encode and decode
    std::mutex _decodeMutex;
    std::mutex _copyEncodeMutex;
    std::mutex _encodeMutex;
    std::thread convertRenderLocalThread;
    std::thread convertRawDataThread;
    std::thread encodeSendThread;
    std::thread decodeRenderThread;
    std::thread convertPartnerThread;
    // for encode & send
    ZRootFrame currentRawFrame;
    std::atomic<bool> hasNewFrameCaptured;
    // for receive & render
    ZEncodedFrame currentEncodedFrame;
    std::atomic<bool> hasReceiveNewFrame;
    // for render ui
    std::shared_ptr<ZLabelRender> _labelRender;
    // for tracker time process
    TimeTracker processLocalConvertTime;
    TimeTracker processEncodeConvertTime;
    TimeTracker processEncodeTime;
    TimeTracker processDecodeConvertTime;
    TimeTracker processDecodeTime;
    TimeTracker processPartnerConvertTime;
    // for queue frame
    ThreadSafeQueue<std::shared_ptr<ZRootFrame>> localQueue;           // for convert local NV12 -> RGB
    ThreadSafeQueue<std::shared_ptr<ZRootFrame>> convertRawDataQueue;  // for convert to send NV12 -> YUV420
    ThreadSafeQueue<std::shared_ptr<ZVideoFrame>> encodeQueue;         // for encode to send
    ThreadSafeQueue<std::shared_ptr<ZEncodedFrame>> decodeQueue;       // for decode partner
    ThreadSafeQueue<std::shared_ptr<ZVideoFrame>> convertPartnerQueue; // for convert to render partner
    // for test
    std::atomic<int> frameCount;
    std::atomic<int> frameCount2;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    std::chrono::time_point<std::chrono::steady_clock> startTime2;
    TimeTracker testTracker;
};

#endif
