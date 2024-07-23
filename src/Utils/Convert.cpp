#include "./Convert.h"

#include "QImage"
#include "fstream"
#include "iostream"

Convert::Convert() {
    srcFrame = av_frame_alloc();
    dstFrame = av_frame_alloc();
    swsCtx = nullptr;
    if (!srcFrame || !dstFrame) {
        std::cerr << "Could not allocate AVFrame." << std::endl;
    }
}

Convert::~Convert() {
    // Giải phóng bộ nhớ
    if (swsCtx) {
        sws_freeContext(swsCtx);
    }
    if (dstFrame) {
        av_freep(&dstFrame->data[0]);
        av_frame_free(&dstFrame);
    }
    if (srcFrame) {
        av_frame_free(&srcFrame);
    }
}

void Convert::convertNV12DatatToYUV420(const std::vector<uchar> &nv12Data,
                                       int width, int height,
                                       std::vector<uchar> &yuv420pData) {
    int frameSize = width * height;
    int yuv420pSize = frameSize + frameSize / 2;
    yuv420pData.resize(yuv420pSize);

    // Con trỏ tới các plane của NV12 và YUV420
    const uchar *yPlane = nv12Data.data();
    const uchar *uvPlane = nv12Data.data() + frameSize;
    memcpy(yuv420pData.data(), yPlane, frameSize);

    // U and V Planes
    uchar *uPlane = yuv420pData.data() + frameSize;
    uchar *vPlane = uPlane + (frameSize / 4);

    int uvWidth = width / 2;
    int uvHeight = height / 2;
    int uvSize = uvWidth * uvHeight;

    for (int i = 0; i < uvSize; ++i) {
        uPlane[i] = uvPlane[i * 2];
        vPlane[i] = uvPlane[i * 2 + 1];
    }
}

// không còn dùng đến
// đã đưa xuống GPU render
void Convert::convertNV12DatatToRGB(const std::vector<uchar> &nv12Data,
                                    int width, int height, QImage &image) {
    int frameSize = width * height;
    const uchar *yPlane = nv12Data.data();
    const uchar *uvPlane = nv12Data.data() + frameSize;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int yIndex = y * width + x;
            int uvIndex = (y / 2) * width + (x & ~1);

            int Y = yPlane[yIndex];
            int U = uvPlane[uvIndex] - 128;
            int V = uvPlane[uvIndex + 1] - 128;

            // Chuyển đổi sử dụng số nguyên
            int C = Y - 16;
            int D = U;
            int E = V;

            int R = (298 * C + 409 * E + 128) >> 8;
            int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
            int B = (298 * C + 516 * D + 128) >> 8;

            // Giới hạn giá trị R, G, B trong khoảng [0, 255]
            R = LIMITVALUE(R);
            G = LIMITVALUE(G);
            B = LIMITVALUE(B);

            image.setPixel(x, y, GETRGB(R, G, B));
        }
    }
}

// không còn dùng đến
// đã đưa xuống GPU render
void Convert::convertYUV420ToRGB(const std::vector<uchar> &yuv420Data,
                                 int width, int height, QImage &image) {
    if (yuv420Data.empty()) {
        qDebug() << "Null yuv420Data pointer.";
        return;
    }

    int frameSize = width * height;
    const uchar *yPlane = yuv420Data.data();
    const uchar *uPlane = yPlane + frameSize;
    const uchar *vPlane = uPlane + (frameSize / 4);
    image = QImage(width, height, QImage::Format_RGB32);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int yIndex = y * width + x;
            int uvIndex = (y / 2) * (width / 2) + (x / 2);

            int Y = yPlane[yIndex];
            int U = uPlane[uvIndex];
            int V = vPlane[uvIndex];

            // Chuyển đổi sử dụng số nguyên
            int C = Y - 16;
            int D = U - 128;
            int E = V - 128;

            int R = (298 * C + 409 * E + 128) >> 8;
            int G = (298 * C - 100 * D - 208 * E + 128) >> 8;
            int B = (298 * C + 516 * D + 128) >> 8;

            // Giới hạn giá trị R, G, B trong khoảng [0, 255]
            R = LIMITVALUE(R);
            G = LIMITVALUE(G);
            B = LIMITVALUE(B);

            image.setPixel(x, y, GETRGB(R, G, B));
        }
    }
}

void Convert::resizeNV12DataCapture(const std::vector<uchar> &inputNv12Data,
                                    int inputWidth, int inputHeight,
                                    std::vector<uchar> &outputNv12Data,
                                    int outputWidth, int outputHeight) {
    if (inputWidth == outputWidth && inputHeight == outputHeight) {
        outputNv12Data = std::move(inputNv12Data);
        return;
    }

    if (!srcFrame || !dstFrame) {
        std::cerr << "Could not allocate AVFrame." << std::endl;
        if (srcFrame)
            av_frame_free(&srcFrame);
        if (dstFrame)
            av_frame_free(&dstFrame);
        return;
    }

    // Thiết lập khung hình nguồn (srcFrame)
    srcFrame->width = inputWidth;
    srcFrame->height = inputHeight;
    srcFrame->format = AV_PIX_FMT_NV12;

    int ret = av_image_fill_arrays(srcFrame->data, srcFrame->linesize,
                                   inputNv12Data.data(), AV_PIX_FMT_NV12,
                                   inputWidth, inputHeight, 1);
    if (ret < 0) {
        std::cerr << "Could not fill srcFrame arrays: " << ret << std::endl;
        av_frame_free(&srcFrame);
        av_frame_free(&dstFrame);
        return;
    }

    // Thiết lập khung hình đích (dstFrame)
    dstFrame->width = outputWidth;
    dstFrame->height = outputHeight;
    dstFrame->format = AV_PIX_FMT_NV12;

    ret = av_image_alloc(dstFrame->data, dstFrame->linesize, outputWidth,
                         outputHeight, AV_PIX_FMT_NV12, 1);
    if (ret < 0) {
        std::cerr << "Could not allocate destination image: " << ret
                  << std::endl;
        av_frame_free(&srcFrame);
        av_frame_free(&dstFrame);
        return;
    }

    // Tạo SwsContext cho việc chuyển đổi định dạng và thay đổi độ phân giải
    swsCtx = sws_getContext(inputWidth, inputHeight, AV_PIX_FMT_NV12,
                            outputWidth, outputHeight, AV_PIX_FMT_NV12,
                            SWS_BILINEAR, nullptr, nullptr, nullptr);

    // Chuyển đổi khung hình
    sws_scale(swsCtx, srcFrame->data, srcFrame->linesize, 0, inputHeight,
              dstFrame->data, dstFrame->linesize);

    // Lưu khung hình đã chuyển đổi vào outputNv12Data
    int outputSize =
        av_image_get_buffer_size(AV_PIX_FMT_NV12, outputWidth, outputHeight, 1);
    outputNv12Data.resize(outputSize);
    av_image_copy_to_buffer(outputNv12Data.data(), outputNv12Data.size(),
                            dstFrame->data, dstFrame->linesize, AV_PIX_FMT_NV12,
                            outputWidth, outputHeight, 1);
}