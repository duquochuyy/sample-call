#include "./DecodeAV1.h"

#include <QDebug>

DecodeAV1::DecodeAV1(int width, int height) : frameCount(0), res(EB_ErrorNone) {
    svtCtx.svtHandle = nullptr;
    svtCtx.outputStreamBuffer = nullptr;
    streamInfo = nullptr;
    frameInfo = nullptr;

    res =
        svt_av1_dec_init_handle(&svtCtx.svtHandle, &svtCtx, &svtCtx.decParams);
    if (res != EB_ErrorNone) {
        std::cerr << "Failed to initialize SVT-AV1 encoder handle" << std::endl;
        return;
    }

    // svtCtx.decParams.max_picture_width = width;
    // svtCtx.decParams.max_picture_height = height;
    svtCtx.decParams.threads = 4;
    svtCtx.decParams.num_p_frames = 1;

    res = svt_av1_dec_set_parameter(svtCtx.svtHandle, &svtCtx.decParams);
    if (res != EB_ErrorNone) {
        std::cerr << "Failed to set SVT-AV1 configuration" << std::endl;
        return;
    }

    res = svt_av1_dec_init(svtCtx.svtHandle);
    if (res != EB_ErrorNone) {
        std::cerr << "Failed to initialize SVT-AV1 decoder" << std::endl;
        return;
    }

    uint32_t lumaSize = width * height;
    uint32_t chromaSize = lumaSize / 4;

    svtCtx.outputStreamBuffer = new EbBufferHeaderType;
    if (svtCtx.outputStreamBuffer == nullptr)
        std::cerr << "Failed to malloc input buffer" << std::endl;
    svtCtx.outputStreamBuffer->size = sizeof(EbBufferHeaderType);
    svtCtx.outputStreamBuffer->p_buffer =
        (uint8_t *)malloc(sizeof(EbSvtIOFormat));
    if (svtCtx.outputStreamBuffer->p_buffer == nullptr) {
        std::cerr << "Failed to malloc IO format buffer" << std::endl;
    }
    svtCtx.outputStreamBuffer->n_alloc_len = width * height * 3 / 2;
    svtCtx.outputStreamBuffer->p_app_private = nullptr;
    svtCtx.outputStreamBuffer->pic_type = EB_AV1_INVALID_PICTURE;
    svtCtx.outputStreamBuffer->metadata = nullptr;

    ((EbSvtIOFormat *)svtCtx.outputStreamBuffer->p_buffer)->luma =
        (uint8_t *)malloc(lumaSize);
    ((EbSvtIOFormat *)svtCtx.outputStreamBuffer->p_buffer)->cb =
        (uint8_t *)malloc(chromaSize);
    ((EbSvtIOFormat *)svtCtx.outputStreamBuffer->p_buffer)->cr =
        (uint8_t *)malloc(chromaSize);

    streamInfo = (EbAV1StreamInfo *)malloc(sizeof(EbAV1StreamInfo));
    frameInfo = (EbAV1FrameInfo *)malloc(sizeof(EbAV1FrameInfo));

    startTime = std::chrono::steady_clock::now();
}

DecodeAV1::~DecodeAV1() {
    if (svtCtx.svtHandle) {
        qDebug() << "start deinit decoder";
        EbErrorType res = svt_av1_dec_deinit(svtCtx.svtHandle);
        if (res != EB_ErrorNone) {
            std::cerr << "Failed to deinitialize SVT-AV1 decoder, error code: "
                      << res << std::endl;
        }
        qDebug() << "end deinit decoder";

        res = svt_av1_dec_deinit_handle(svtCtx.svtHandle);
        if (res != EB_ErrorNone) {
            std::cerr
                << "Failed to deinitialize SVT-AV1 decoder handle, error code: "
                << res << std::endl;
        }

        svtCtx.svtHandle = nullptr;
    }
    if (svtCtx.outputStreamBuffer) {
        if (svtCtx.outputStreamBuffer->p_buffer) {
            free(svtCtx.outputStreamBuffer->p_buffer);
            svtCtx.outputStreamBuffer->p_buffer = nullptr;
        }

        delete svtCtx.outputStreamBuffer;
        svtCtx.outputStreamBuffer = nullptr;
    }

    if (streamInfo) {
        free(streamInfo);
        streamInfo = nullptr;
    }

    if (frameInfo) {
        free(frameInfo);
        frameInfo = nullptr;
    }
}

void DecodeAV1::decode(const std::vector<uchar> &encodedData,
                       const uint64_t timestamp,
                       std::shared_ptr<ZVideoFrame> &decodedFrame) {
    if (encodedData.size() == 0) {
        // std::cerr << "Input frame data is empty" << std::endl;
        return;
    }

    res = svt_av1_dec_frame(svtCtx.svtHandle, encodedData.data(),
                            encodedData.size(), 0);
    if (res != EB_ErrorNone) {
        std::cerr << "Error decoding frame" << std::endl;
        return;
    }

    res = svt_av1_dec_get_picture(svtCtx.svtHandle, svtCtx.outputStreamBuffer,
                                  streamInfo, frameInfo);
    if (res != EB_ErrorNone) {
        std::cerr << "Error getting decoded picture" << std::endl;
        return;
    }

    frameBuffer = (EbSvtIOFormat *)svtCtx.outputStreamBuffer->p_buffer;

    uint32_t lumaSize = frameBuffer->width * frameBuffer->height;
    uint32_t chromaSize = lumaSize / 4;

    decodedFrame->width = frameBuffer->width;
    decodedFrame->height = frameBuffer->height;
    decodedFrame->timestamp = timestamp;
    if (decodedFrame->yuv420pData.size() < lumaSize + 2 * chromaSize) {
        decodedFrame->yuv420pData.resize(lumaSize + 2 * chromaSize);
    }

    std::copy(frameBuffer->luma, frameBuffer->luma + lumaSize,
              decodedFrame->yuv420pData.begin());
    std::copy(frameBuffer->cb, frameBuffer->cb + chromaSize,
              decodedFrame->yuv420pData.begin() + lumaSize);
    std::copy(frameBuffer->cr, frameBuffer->cr + chromaSize,
              decodedFrame->yuv420pData.begin() + lumaSize + chromaSize);

    // std::cout << "end decode" << std::endl;

    getInfo(frameBuffer->width, frameBuffer->height);
}

void DecodeAV1::getInfo(int width, int height) {
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

void DecodeAV1::disconnect() {}
