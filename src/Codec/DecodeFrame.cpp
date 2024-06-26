#include "./DecodeFrame.h"

DecodeFrame::DecodeFrame(int width, int height) : frameCount(0)
{
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec)
    {
        qDebug() << "Codec not found";
        return;
    }
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext)
    {
        qDebug() << "Failed to allocate codec context";
        return;
    }
    codecContext->width = width;
    codecContext->height = height;
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    if (avcodec_open2(codecContext, codec, nullptr) < 0)
    {
        qDebug() << "Failed to open codec";
        avcodec_free_context(&codecContext);
        return;
    }

    frame = av_frame_alloc();
    if (!frame)
    {
        qDebug() << "Failed to allocate AVFrame";
        avcodec_free_context(&codecContext);
        return;
    }

    packet = av_packet_alloc();
    if (!packet)
    {
        qDebug() << "Could not allocate AVPacket";
        av_frame_free(&frame);
        avcodec_free_context(&codecContext);
        return;
    }
}

DecodeFrame::~DecodeFrame()
{
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecContext);
}

void DecodeFrame::registerCallback(Callback *callback)
{
    _callback = callback;
}

void DecodeFrame::decodeH264ToYUV420(const std::vector<uchar> &encodedData, const uint64_t timestamp, std::shared_ptr<ZVideoFrame> &decodedFrame)
{
    if (encodedData.empty())
    {
        qDebug() << "Encoded data is empty";
        return;
    }

    av_packet_unref(packet);
    av_frame_unref(frame);

    packet->data = const_cast<uchar *>(encodedData.data());
    packet->size = encodedData.size();

    int ret = avcodec_send_packet(codecContext, packet);
    if (ret < 0)
    {
        qDebug() << "Error sending a packet for decoding: " << av_err2str(ret);
        return;
    }

    ret = avcodec_receive_frame(codecContext, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
    {
        av_packet_free(&packet);
        return;
    }
    else if (ret < 0)
    {
        qDebug() << "Error during decoding: " << av_err2str(ret);
        av_packet_free(&packet);
        return;
    }

    int y_size = frame->width * frame->height;
    int uv_size = frame->width * frame->height / 4;
    int frame_size = y_size + 2 * uv_size;

    decodedFrame.get()->width = frame->width;
    decodedFrame.get()->height = frame->height;
    decodedFrame.get()->timestamp = timestamp;

    decodedFrame.get()->yuv420pData.resize(y_size + 2 * uv_size);

    std::memcpy(decodedFrame.get()->yuv420pData.data(), frame->data[0], y_size);                     // Copy Y plane
    std::memcpy(decodedFrame.get()->yuv420pData.data() + y_size, frame->data[1], uv_size);           // Copy U plane
    std::memcpy(decodedFrame.get()->yuv420pData.data() + y_size + uv_size, frame->data[2], uv_size); // Copy V plane
    getInfo(frame->width, frame->height);
}

void DecodeFrame::getInfo(int width, int height)
{
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0)
    {
        int fps = frameCount.load() / elapsedSeconds.count();
        frameCount = 0;
        startTime = currentTime;

        if (_callback != nullptr)
            _callback->onShowInfoDecode(width, height, fps);
    }
}