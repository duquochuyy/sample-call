#include "./DecodeH264.h"

DecodeH264::DecodeH264(int width, int height)
    : frameCount(0), codecContext(nullptr), frame(nullptr), packet(nullptr) {
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        std::cerr << "Codec not found" << std::endl;
        return;
    }
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        std::cerr << "Failed to allocate codec context" << std::endl;
        return;
    }
    codecContext->width = width;
    codecContext->height = height;
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        std::cerr << "Failed to open codec" << std::endl;
        avcodec_free_context(&codecContext);
        codecContext = nullptr;
        return;
    }

    // buffer for decode data
    frame = av_frame_alloc();
    if (!frame) {
        std::cerr << "Failed to allocate AVFrame" << std::endl;
        avcodec_free_context(&codecContext);
        codecContext = nullptr;
        return;
    }

    // buffer for encode data
    packet = av_packet_alloc();
    if (!packet) {
        std::cerr << "Could not allocate AVPacket" << std::endl;
        av_frame_free(&frame);
        frame = nullptr;
        avcodec_free_context(&codecContext);
        codecContext = nullptr;
        return;
    }
}

DecodeH264::~DecodeH264() { disconnect(); }

void DecodeH264::decode(const std::vector<uchar> &encodedData,
                        const uint64_t timestamp,
                        std::shared_ptr<ZVideoFrame> &decodedFrame) {
    if (encodedData.empty()) {
        std::cerr << "Encoded data is empty" << std::endl;
        return;
    }

    av_packet_unref(packet);
    av_frame_unref(frame);

    packet->data = const_cast<uchar *>(encodedData.data());
    packet->size = encodedData.size();

    int ret = avcodec_send_packet(codecContext, packet);
    if (ret < 0) {
        std::cerr << "Error sending a packet for decoding: " << av_err2str(ret)
                  << std::endl;
        return;
    }

    ret = avcodec_receive_frame(codecContext, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        av_packet_free(&packet);
        return;
    } else if (ret < 0) {
        std::cerr << "Error during decoding: " << av_err2str(ret) << std::endl;
        av_packet_free(&packet);
        return;
    }

    int y_size = frame->width * frame->height;
    int uv_size = frame->width * frame->height / 4;
    int frame_size = y_size + 2 * uv_size;

    decodedFrame.get()->width = frame->width;
    decodedFrame.get()->height = frame->height;
    decodedFrame.get()->timestamp = timestamp;

    if (decodedFrame->yuv420pData.size() < frame_size) {
        decodedFrame->yuv420pData.resize(frame_size);
    }

    std::copy(frame->data[0], frame->data[0] + y_size,
              decodedFrame->yuv420pData.begin());  // Y plane
    std::copy(frame->data[1], frame->data[1] + uv_size,
              decodedFrame->yuv420pData.begin() + y_size);  // U plane
    std::copy(frame->data[2], frame->data[2] + uv_size,
              decodedFrame->yuv420pData.begin() + y_size + uv_size);  // V plane

    getInfo(frame->width, frame->height);
}

void DecodeH264::getInfo(int width, int height) {
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0) {
        int fps = frameCount.load() / elapsedSeconds.count();
        frameCount = 0;
        startTime = currentTime;

        if (_callback != nullptr)
            _callback->onShowInfoDecode(width, height, fps);
    }
}

void DecodeH264::disconnect() {
    // if (packet) {
    //     av_packet_free(&packet);
    //     packet = nullptr;  // Đảm bảo không giải phóng lại
    // }
    // if (frame) {
    //     av_frame_free(&frame);
    //     frame = nullptr;  // Đảm bảo không giải phóng lại
    // }
    // if (codecContext) {
    //     avcodec_free_context(&codecContext);
    //     codecContext = nullptr;  // Đảm bảo không giải phóng lại
    // }
}