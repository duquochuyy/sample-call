#include "./EncodeFrame.h"

EncodeFrame::EncodeFrame(int width, int height) : _width(width), _height(height), encoder(nullptr), frameCount(0)
{
    x264_param_default_preset(&param, "veryfast", "zerolatency");
    param.i_threads = X264_SYNC_LOOKAHEAD_AUTO;
    // param.i_threads = 1;
    param.i_csp = X264_CSP_I420;
    param.i_width = width;
    param.i_height = height;
    param.i_fps_num = 30;
    param.i_fps_den = 1;
    param.i_keyint_max = 60;
    param.b_intra_refresh = 1;
    param.rc.i_rc_method = X264_RC_CRF;
    param.rc.f_rf_constant = 23;
    param.rc.i_vbv_buffer_size = 5000000;
    param.rc.i_vbv_max_bitrate = 5000000;
    param.b_repeat_headers = 1;
    param.b_annexb = 1;

    encoder = x264_encoder_open(&param);
    if (!encoder)
    {
        qDebug() << "Failed to open x264 encoder";
        return;
    }
    x264_picture_alloc(&pic, X264_CSP_I420, width, height);
}

EncodeFrame::~EncodeFrame()
{
    if (encoder)
    {
        x264_encoder_close(encoder);
    }
    x264_picture_clean(&pic);
}

void EncodeFrame::registerCallback(Callback *callback)
{
    _callback = callback;
}

void EncodeFrame::encodeYUV420ToH264(const ZVideoFrame &frame, ZEncodedFrame &encodedFrame)
{
    x264_picture_init(&pic);
    pic.img.i_csp = X264_CSP_I420;
    pic.img.i_plane = 3;
    pic.img.i_stride[0] = frame.width;
    pic.img.i_stride[1] = frame.width / 2;
    pic.img.i_stride[2] = frame.width / 2;
    pic.img.plane[0] = const_cast<uint8_t *>(frame.yuv420pData.data());
    pic.img.plane[1] = pic.img.plane[0] + frame.width * frame.height;
    pic.img.plane[2] = pic.img.plane[1] + frame.width * frame.height / 4;
    pic.i_pts = (frame.timestamp * TIMEBASE) / 1000;

    x264_nal_t *nal;
    int i_nal;
    int frameSize = x264_encoder_encode(encoder, &nal, &i_nal, &pic, &pic_out);

    if (frameSize < 0)
    {
        qDebug() << "Error encode";
        return;
    }

    encodedFrame.frameSize = frameSize;
    encodedFrame.width = frame.width;
    encodedFrame.height = frame.height;
    encodedFrame.timestamp = frame.timestamp;

    for (int i = 0; i < i_nal; ++i)
    {
        encodedFrame.encodedData.insert(encodedFrame.encodedData.end(), nal[i].p_payload, nal[i].p_payload + nal[i].i_payload);
    }
    getInfo(_width, _height);
}

void EncodeFrame::getInfo(int width, int height)
{
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0)
    {
        int fps = frameCount.load() / elapsedSeconds.count();
        frameCount = 0;
        startTime = currentTime;

        if (_callback != nullptr)
            _callback->onShowInfoEncode(width, height, fps);
    }
}
