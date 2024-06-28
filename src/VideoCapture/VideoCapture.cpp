#include "./VideoCapture.h"

void VideoCapture::registerCallback(Callback *callback) {
    _callback = callback;
}
