#include "./QtVideoRender.h"

QtVideoRender::QtVideoRender()
{
}

QtVideoRender::~QtVideoRender()
{
    delete _label;
}

void QtVideoRender::setVideoFrameLabel(QLabel *label)
{
    _label = label;
}

QImage QtVideoRender::convertYUV420ToRGB(const std::vector<uchar> &yuv420Data, int width, int height)
{
    if (yuv420Data.empty())
    {
        qDebug() << "Null yuv420Data pointer.";
        return QImage();
    }

    int frameSize = width * height;
    const uchar *yPlane = yuv420Data.data();
    const uchar *uPlane = yPlane + frameSize;
    const uchar *vPlane = uPlane + (frameSize / 4);
    QImage rgbImage(width, height, QImage::Format_RGB32);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int yIndex = y * width + x;
            int uvIndex = (y / 2) * (width / 2) + (x / 2);

            int Y = yPlane[yIndex];
            int U = uPlane[uvIndex] - 128;
            int V = vPlane[uvIndex] - 128;

            int R = qBound(0, (int)(Y + 1.402 * V), 255);
            int G = qBound(0, (int)(Y - 0.344136 * U - 0.714136 * V), 255);
            int B = qBound(0, (int)(Y + 1.772 * U), 255);

            rgbImage.setPixel(x, y, qRgb(R, G, B));
        }
    }
    // rgbImage.save("abc.jpg");
    return rgbImage;
}

void QtVideoRender::render(const std::shared_ptr<ZVideoFrame> &frame)
{
    QImage frameConverted = convertYUV420ToRGB(frame->yuv420pData, frame->width, frame->height);
    if (frameConverted.isNull())
    {
        qDebug() << "Local Converted frame is null.";
        return;
    }
    if (_label != nullptr)
    {
        _label->setPixmap(QPixmap::fromImage(frameConverted).scaled(_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
