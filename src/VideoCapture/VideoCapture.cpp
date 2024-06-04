#include "./VideoCapture.h"

//VideoCapture::VideoCapture() {}

void VideoCapture::registerCallback(Callback *callback)
{
    _callback = callback;
}
