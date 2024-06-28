#include "./VideoRender.h"

VideoRender::VideoRender() { _convert.reset(new Convert()); }

void VideoRender::registerCallback(Callback *callback) { _callback = callback; }
