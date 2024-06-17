#ifndef INFONETWORK_H
#define INFONETWORK_H

#include <QLabel>

class InfoNetwork
{
public:
    InfoNetwork();
    ~InfoNetwork();
    void setLabel(QLabel *width, QLabel *height, QLabel *fps, QLabel *bitrate);
    void renderInfo(int width, int height, int fps, double bitrate);

private:
    QLabel *_labelWidth;
    QLabel *_labelHeight;
    QLabel *_labelFps;
    QLabel *_labelBitrate;
};

#endif