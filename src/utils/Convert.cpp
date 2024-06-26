#include "./Convert.h"

void Convert::processNV12DatatToYUV420(const std::vector<uchar> &nv12Data,
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

void Convert::processNV12DatatToRGB(const std::vector<uchar> &nv12Data,
                                    int width, int height, QImage &image) {
    int frameSize = width * height;
    const uchar *yPlane = nv12Data.data();
    const uchar *uvPlane = nv12Data.data() + frameSize;

    // Convert to RGB
    image = QImage(width, height, QImage::Format_RGB32);

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