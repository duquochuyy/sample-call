#ifndef ZLABELRENDER_H
#define ZLABELRENDER_H

#include <QLabel>

struct ZValueInfo {
    // capture
    int captureWidth;
    int captureHeight;
    int captureFps;
    int localConvertTime;
    int localFps;
    // encode
    int encodeWidth;
    int encodeHeight;
    int encodeFps;
    int encodeConvertTime;
    int encodeTime;
    int encodeScaleTime = 0.0;
    // send
    int sendFps;
    int sendPps;
    double sendBitrate;
    // receive
    int receiveFps;
    int receivePps;
    double receiveBitrate;
    // decode
    int decodeWidth;
    int decodeHeight;
    int decodeFps;
    int decodeConvertTime;
    int decodeTime;
    int decodeScaleTime = 0.0;
    // partner render
    int partnerFps;
    int partConvertTime;

    ZValueInfo() {}
    ZValueInfo(int cWidth, int cHeight, int cFps, int lConvertTime, int lFps,
               int eWidth, int eHeight, int eFps, int eConvertTime, int eTime,
               int sTime, int sFps, int sPps, double sBitrate, int rFps,
               int rPps, double rBitrate, int dWidth, int dHeight, int dFps,
               int dConvertTime, int dTime, int dScaleTime, int pFps,
               int pConvertTime)
        : captureWidth(cWidth),
          captureHeight(cHeight),
          captureFps(cFps),
          localConvertTime(lConvertTime),
          localFps(lFps),
          encodeWidth(eWidth),
          encodeHeight(eHeight),
          encodeFps(eFps),
          encodeConvertTime(eConvertTime),
          encodeTime(eTime),
          encodeScaleTime(sTime),
          sendFps(sFps),
          sendPps(sPps),
          sendBitrate(sBitrate),
          receiveFps(rFps),
          receivePps(rPps),
          receiveBitrate(rBitrate),
          decodeWidth(dWidth),
          decodeHeight(dHeight),
          decodeFps(dFps),
          decodeConvertTime(dConvertTime),
          decodeTime(dTime),
          decodeScaleTime(dScaleTime),
          partnerFps(pFps),
          partConvertTime(pConvertTime) {}
};

#endif