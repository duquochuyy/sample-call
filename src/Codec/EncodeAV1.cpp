#include "./EncodeAV1.h"

#include <QDebug>
#include <chrono>
#include <thread>

EncodeAV1::EncodeAV1(int width, int height, int bitrate)
    : frameCount(0),
      res(EB_ErrorNone),
      frameBuffer(nullptr),
      hasNewPacket(false) {
    svtCtx.svtHandle = nullptr;
    svtCtx.inputPictureBuffer = nullptr;
    svtCtx.outputStreamBuffer = nullptr;
    // Create and config codec
    res =
        svt_av1_enc_init_handle(&svtCtx.svtHandle, &svtCtx, &svtCtx.encParams);
    if (res != EB_ErrorNone) {
        std::cerr << "Failed to initialize SVT-AV1 encoder handle, error code: "
                  << res << std::endl;
        return;
    }
    qDebug() << width << height << bitrate;
    svtCtx.encParams.source_width = width;
    svtCtx.encParams.source_height = height;
    svtCtx.encParams.rate_control_mode = SVT_AV1_RC_MODE_VBR;
    svtCtx.encParams.target_bit_rate = bitrate;
    svtCtx.encParams.tile_columns = 3;
    svtCtx.encParams.tile_rows = 2;
    svtCtx.encParams.enc_mode = 13;
    svtCtx.encParams.encoder_bit_depth = 8;
    svtCtx.encParams.encoder_color_format = EB_YUV420;
    svtCtx.encParams.frame_rate_numerator = 30;
    svtCtx.encParams.frame_rate_denominator = 1;
    svtCtx.encParams.profile = MAIN_PROFILE;
    svtCtx.encParams.look_ahead_distance = 0;  // số frame nhìn cho tương lai
    // svtCtx.encParams.fast_decode = 1;
    // svtCtx.encParams.under_shoot_pct = 100;
    // svtCtx.encParams.over_shoot_pct = 100;
    svtCtx.encParams.hierarchical_levels = 2;

    svtCtx.inputPictureBuffer = new EbBufferHeaderType;
    if (svtCtx.inputPictureBuffer == nullptr)
        std::cerr << "Failed to malloc input buffer" << std::endl;
    svtCtx.inputPictureBuffer->size = sizeof(EbBufferHeaderType);
    svtCtx.inputPictureBuffer->p_buffer = nullptr;
    svtCtx.inputPictureBuffer->p_app_private = nullptr;
    svtCtx.inputPictureBuffer->pic_type = EB_AV1_INVALID_PICTURE;
    svtCtx.inputPictureBuffer->metadata = nullptr;

    res = svt_av1_enc_set_parameter(svtCtx.svtHandle, &svtCtx.encParams);
    if (res != EB_ErrorNone) {
        std::cerr << "Failed to set SVT-AV1 configuration, error code: " << res
                  << std::endl;
        return;
    }

    res = svt_av1_enc_init(svtCtx.svtHandle);
    if (res != EB_ErrorNone) {
        std::cerr << "Failed to initialize SVT-AV1 encoder, error code: " << res
                  << std::endl;
        return;
    }

    frameBuffer = (EbSvtIOFormat *)malloc(sizeof(EbSvtIOFormat));
    if (frameBuffer == nullptr) {
        std::cerr << "Failed to create frame buffer" << std::endl;
        return;
    }
    memset(frameBuffer, 0, sizeof(EbSvtIOFormat));
    frameBuffer->width = width;
    frameBuffer->height = height;
    frameBuffer->org_x = 0;
    frameBuffer->org_y = 0;
    frameBuffer->color_fmt = EB_YUV420;
    frameBuffer->bit_depth = EB_EIGHT_BIT;
    frameBuffer->y_stride = width;
    frameBuffer->cb_stride = width / 2;
    frameBuffer->cr_stride = width / 2;
    uint32_t lumaSize = width * height;
    uint32_t chromaSize = lumaSize / 4;
    frameBuffer->luma = (uint8_t *)malloc(lumaSize);
    memset(frameBuffer->luma, 0, lumaSize);
    frameBuffer->cb = (uint8_t *)malloc(chromaSize);
    memset(frameBuffer->cb, 0, chromaSize);
    frameBuffer->cr = (uint8_t *)malloc(chromaSize);
    memset(frameBuffer->cr, 0, chromaSize);

    startTime = std::chrono::steady_clock::now();

    isEncoding = true;
}

EncodeAV1::~EncodeAV1() {
    if (svtCtx.inputPictureBuffer) {
        // send eos packet
        svtCtx.inputPictureBuffer->flags = EB_BUFFERFLAG_EOS;
        svtCtx.inputPictureBuffer->p_buffer = nullptr;
        svtCtx.inputPictureBuffer->n_filled_len = 0;

        EbErrorType res = svt_av1_enc_send_picture(svtCtx.svtHandle,
                                                   svtCtx.inputPictureBuffer);
        if (res != EB_ErrorNone) {
            std::cerr << "Failed to send EOS picture: " << res << std::endl;
        } else {
            while (true) {
                res = svt_av1_enc_get_packet(svtCtx.svtHandle,
                                             &svtCtx.outputStreamBuffer, 0);
                if (res == EB_NoErrorEmptyQueue) {
                    break;
                } else if (res != EB_ErrorNone) {
                    std::cerr
                        << "Failed to get packet after sending EOS: " << res
                        << std::endl;
                }
            }
        }
    }

    if (svtCtx.svtHandle) {
        EbErrorType res = svt_av1_enc_deinit(svtCtx.svtHandle);
        if (res != EB_ErrorNone) {
            std::cerr << "Failed to deinitialize SVT-AV1 encoder, error code: "
                      << res << std::endl;
        }
        res = svt_av1_enc_deinit_handle(svtCtx.svtHandle);
        if (res != EB_ErrorNone) {
            std::cerr
                << "Failed to deinitialize SVT-AV1 encoder handle, error code: "
                << res << std::endl;
        }
        svtCtx.svtHandle = nullptr;
    }

    // if (svtCtx.outputStreamBuffer) {
    //     if (svtCtx.outputStreamBuffer->p_buffer) {
    //         delete[] svtCtx.outputStreamBuffer->p_buffer;
    //         svtCtx.outputStreamBuffer->p_buffer = nullptr;
    //     }
    //     delete svtCtx.outputStreamBuffer;
    //     svtCtx.outputStreamBuffer = nullptr;
    // }

    if (svtCtx.inputPictureBuffer) {
        delete svtCtx.inputPictureBuffer;
        svtCtx.inputPictureBuffer = nullptr;
    }
}

void EncodeAV1::encode(const ZVideoFrame &frame, ZEncodedFrame &encodedFrame) {
    if (frame.yuv420pData.empty()) {
        // std::cerr << "Input frame data is empty" << std::endl;
        return;
    }

    uint32_t lumaSize = frame.width * frame.height;
    uint32_t chromaSize = lumaSize / 4;
    frameBuffer->width = frame.width;
    frameBuffer->height = frame.height;
    frameBuffer->bit_depth = EB_EIGHT_BIT;
    frameBuffer->color_fmt = EB_YUV420;

    frameBuffer->luma = const_cast<uint8_t *>(frame.yuv420pData.data());
    frameBuffer->cb = frameBuffer->luma + frame.width * frame.height;
    frameBuffer->cr = frameBuffer->cb + frame.width * frame.height / 4;

    svtCtx.inputPictureBuffer->p_buffer = (uint8_t *)frameBuffer;
    svtCtx.inputPictureBuffer->n_filled_len = frame.yuv420pData.size();
    svtCtx.inputPictureBuffer->pts = (frame.timestamp * TIMEBASE) / 1000;
    svtCtx.inputPictureBuffer->flags = 0;

    res = svt_av1_enc_send_picture(svtCtx.svtHandle, svtCtx.inputPictureBuffer);
    if (res != EB_ErrorNone) {
        std::cerr << "Failed to send picture: " << res << std::endl;
        return;
    }

    svtCtx.outputStreamBuffer = nullptr;
    EbErrorType res =
        svt_av1_enc_get_packet(svtCtx.svtHandle, &svtCtx.outputStreamBuffer, 0);
    if (res == EB_ErrorMax) {
        std::cerr << "Failed to get packet from encoder: " << res << " "
                  << svtCtx.outputStreamBuffer << std::endl;
        return;
    } else if (res == EB_NoErrorEmptyQueue) {
        std::cout << "No packet available in encoder queue" << std::endl;
        return;
    }
    if (svtCtx.outputStreamBuffer == nullptr) {
        std::cout << "Has output null packet";
        return;
    }
    std::cout << "Packet received, size: "
              << svtCtx.outputStreamBuffer->n_filled_len << std::endl;

    encodedFrame.frameSize = svtCtx.outputStreamBuffer->n_filled_len;
    encodedFrame.width = frame.width;
    encodedFrame.height = frame.height;
    encodedFrame.timestamp = (svtCtx.outputStreamBuffer->pts) * 1000 / TIMEBASE;
    encodedFrame.encodedData.assign(
        svtCtx.outputStreamBuffer->p_buffer,
        svtCtx.outputStreamBuffer->p_buffer +
            svtCtx.outputStreamBuffer->n_filled_len);

    svt_av1_enc_release_out_buffer(&svtCtx.outputStreamBuffer);
    svtCtx.outputStreamBuffer = nullptr;
    getInfo(frame.width, frame.height);
}

void EncodeAV1::getPacket() {
    while (isEncoding) {
        if (hasNewPacket) {
            std::this_thread::sleep_for(
                std::chrono::duration<double, std::milli>(1));
        }
        std::cout << "is getting packet";
        outputPacket = nullptr;
        EbErrorType res =
            svt_av1_enc_get_packet(svtCtx.svtHandle, &outputPacket, 0);
        if (res == EB_ErrorMax) {
            std::cerr << "Failed to get packet from encoder: " << res << " "
                      << outputPacket << std::endl;
            return;
        } else if (res == EB_NoErrorEmptyQueue) {
            std::cout << "No packet available in encoder queue" << std::endl;
            continue;
        }
        if (outputPacket == nullptr) {
            std::cout << "Has output null packet";
            return;
        }
        std::cout << "Packet received, size: " << outputPacket->n_filled_len
                  << std::endl;

        svtCtx.outputStreamBuffer = outputPacket;
        hasNewPacket = true;
    }
}

void EncodeAV1::getInfo(int width, int height) {
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0) {
        int fps = frameCount.load() / elapsedSeconds.count();
        frameCount = 0;
        startTime = currentTime;

        if (_callback != nullptr)
            _callback->onShowInfoEncode(width, height, fps);
    }
}

void EncodeAV1::disconnect() {}
