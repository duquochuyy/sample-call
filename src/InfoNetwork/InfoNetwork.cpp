#include "./InfoNetwork.h"

InfoNetwork::InfoNetwork()
{
}

InfoNetwork::~InfoNetwork()
{
    delete _labelWidth;
    delete _labelHeight;
    delete _labelFps;
    delete _labelBitrate;
}

void InfoNetwork::setLabel(QLabel *width, QLabel *height, QLabel *fps, QLabel *bitrate)
{
    _labelWidth = width;
    _labelHeight = height;
    _labelFps = fps;
    _labelBitrate = bitrate;
}

void InfoNetwork::renderInfo(int width, int height, int fps, double bitrate)
{
    _labelWidth->setText(QString::number(width));
    _labelHeight->setText(QString::number(height));
    _labelFps->setText(QString::number(fps));
    _labelBitrate->setText(QString::number(bitrate));
}
