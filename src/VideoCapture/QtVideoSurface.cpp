#include "./QtVideoSurface.h"

QTVideoSurface::QTVideoSurface(QObject *parent) : QAbstractVideoSurface(parent) {}

QList<QVideoFrame::PixelFormat> QTVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    return {QVideoFrame::Format_YUV420P, QVideoFrame::Format_YV12, QVideoFrame::Format_NV12, QVideoFrame::Format_NV21, QVideoFrame::Format_YUV422P, QVideoFrame::Format_YUYV, QVideoFrame::Format_YUV444};
}

bool QTVideoSurface::present(const QVideoFrame &frame)
{
    if (frame.isValid())
    {
//         frame.image().save("original.jpg");

        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);

        if (cloneFrame.pixelFormat() == QVideoFrame::Format_NV12)
        {
            int width = cloneFrame.width();
            int height = cloneFrame.height();
            const uchar *nv12Data = cloneFrame.bits();
            const uchar *yuv420pData = processNV12DatatToYUV420(nv12Data, width, height);
            emit frameCaptured(yuv420pData, width, height);
        }

        cloneFrame.unmap();
        return true;
    }
    return false;
}

bool QTVideoSurface::start(const QVideoSurfaceFormat &format)
{
    qDebug() << "Starting video surface with format:" << format.pixelFormat();
    return QAbstractVideoSurface::start(format);
}

void QTVideoSurface::stop()
{
    qDebug() << "Stopping video surface";
    QAbstractVideoSurface::stop();
}

void QTVideoSurface::processYUVDataToGrayScale(const uchar *yuvData, int width, int height)
{
    int frameSize = width * height;
    const uchar *yPlane = yuvData;
    const uchar *uPlane = yuvData + frameSize;
    const uchar *vPlane = yuvData + frameSize + (frameSize / 4);

    // convert to
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

    // Example: Convert to grayscale
    QImage grayImage = rgbImage.convertToFormat(QImage::Format_Grayscale8);
    qDebug() << "Converted image to grayscale";
    // Further processing can be done here
    grayImage.save("abc.jpg");
}

uchar *QTVideoSurface::processNV12DatatToYUV420(const uchar *nv12Data, int width, int height)
{
    int frameSize = width * height;
    int yuv420pSize = frameSize + frameSize / 2;
    uchar *yuv420pData = new uchar[yuv420pSize];
    const uchar *yPlane = nv12Data;
    const uchar *uvPlane = nv12Data + frameSize;
    memcpy(yuv420pData, yPlane, frameSize);

    // U and V Planes
    uchar *uPlane = yuv420pData + frameSize;
    uchar *vPlane = uPlane + (frameSize / 4);

    for (int y = 0; y < height / 2; ++y)
    {
        for (int x = 0; x < width / 2; ++x)
        {
            int uvIndex = y * width + 2 * x;
            uPlane[y * (width / 2) + x] = uvPlane[uvIndex];
            vPlane[y * (width / 2) + x] = uvPlane[uvIndex + 1];
        }
    }

    return yuv420pData;
}
