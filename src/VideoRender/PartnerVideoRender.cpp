#include "./PartnerVideoRender.h"

PartnerVideoRender::PartnerVideoRender()
{
}

PartnerVideoRender::~PartnerVideoRender()
{
    delete _label;
}

void PartnerVideoRender::setVideoFrameLabel(QLabel *label)
{
    _label = label;
}

QImage PartnerVideoRender::convertYUV420ToRGB(const uchar *yuv240Data, int width, int height)
{
    int frameSize = width * height;
    const uchar *yPlane = yuv240Data;
    const uchar *uPlane = yuv240Data + frameSize;
    const uchar *vPlane = yuv240Data + frameSize + (frameSize / 4);
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
    // if (!rgbImage.save("partner.jpg"))
    // {
    //     qDebug() << "Failed to save image to";
    // }
    // else
    // {
    //     qDebug() << "Image saved to";
    // }

    return rgbImage;
}

void PartnerVideoRender::render(const ZVideoFrame &frame)
{
    QImage frameConverted = convertYUV420ToRGB(frame.yuv420pData, frame.width, frame.height);
    if (_label != nullptr)
    {
        _label->setPixmap(QPixmap::fromImage(frameConverted).scaled(_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        qDebug() << "render frame" << QString::number(frame.timestamp);
    }
}
