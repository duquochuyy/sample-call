#include "./NetworkSender.h"

NetworkSender::NetworkSender(int localPort) : localPort(localPort), sock(-1), isSending(false), totalBytesSent(0), frameCount(0)
{
}

NetworkSender::~NetworkSender()
{
    disconnect();
}

void NetworkSender::registerCallback(Callback *callback)
{
    _callback = callback;
}

bool NetworkSender::handleConnectPartner(std::string ip, int port)
{
    struct sockaddr_in serv_addr;
    // creata socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        qDebug() << "Socket creation error";
        return false;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0)
    {
        qDebug() << "Invalid address/ Address not supported";
        return false;
    }
    // connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        qDebug() << "Connection Failed";
        return false;
    }
    {
        send(sock, &localPort, sizeof(localPort), 0);
    }
    isSending = true;
    startSending();
    return true;
}

void NetworkSender::addNewFrame(const ZVideoFrame &frame)
{
    const std::lock_guard<std::mutex> lock(frameMutex);
    currentFrame = frame;
    hasNewFrame = true;
}

void NetworkSender::addNewEncodedFrame(const ZEncodedFrame &encodedFrame)
{
    const std::lock_guard<std::mutex> lock(encodedFrameMutex);
    currentEncodedFrame = encodedFrame;
    hasNewFrame = true;
}

void NetworkSender::disconnect()
{
    if (isSending)
    {
        std::string disconnectMessage = "disconnect";
        send(sock, disconnectMessage.c_str(), disconnectMessage.size(), 0);
        isSending = false;
        if (sock >= 0)
        {
            close(sock);
            sock = -1;
        }
    }
}

void NetworkSender::sendData()
{
    while (isSending)
    {
        if (!hasNewFrame)
        {
            std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1));
            continue;
        }

        const std::lock_guard<std::mutex> lock(encodedFrameMutex);

        int dataSize = currentEncodedFrame.encodedData.size();
        int headerSize = sizeof(uint64_t) + 5 * sizeof(int);
        int totalPackets = (dataSize + (PACKET_SIZE - 1) - headerSize) / (PACKET_SIZE - headerSize);
        int offset = 0;

        for (int packetId = 0; packetId < totalPackets; ++packetId)
        {
            int chunkSize = std::min(static_cast<int>(PACKET_SIZE - headerSize), dataSize - offset);
            std::vector<char> packet(PACKET_SIZE);

            std::memcpy(packet.data(), &currentEncodedFrame.timestamp, sizeof(uint64_t));

            std::memcpy(packet.data() + sizeof(uint64_t), &totalPackets, sizeof(int));

            std::memcpy(packet.data() + sizeof(uint64_t) + sizeof(int), &packetId, sizeof(int));

            std::memcpy(packet.data() + sizeof(uint64_t) + 2 * sizeof(int), &currentEncodedFrame.width, sizeof(int));

            std::memcpy(packet.data() + sizeof(uint64_t) + 3 * sizeof(int), &currentEncodedFrame.height, sizeof(int));

            std::memcpy(packet.data() + sizeof(uint64_t) + 4 * sizeof(int), &chunkSize, sizeof(int));

            std::memcpy(packet.data() + sizeof(uint64_t) + 5 * sizeof(int), currentEncodedFrame.encodedData.data() + offset, chunkSize);

            offset += chunkSize;

            send(sock, packet.data(), packet.size(), 0);

            totalBytesSent += packet.size();
        }

        // increase frame for send
        frameCount++;
        getInfoSend();
        hasNewFrame = false;
    }
}

void NetworkSender::startSending()
{
    startTime = std::chrono::steady_clock::now();
    sendThread = std::thread([this]()
                             {
                                  while (true)
                                  {
                                        sendData();
                                  } });
    sendThread.detach();
}

void NetworkSender::testShowImage(uchar *yuv420pData, int width, int height)
{
    int frameSize = width * height;
    const uchar *yPlane = yuv420pData;
    const uchar *uPlane = yuv420pData + frameSize;
    const uchar *vPlane = yuv420pData + frameSize + (frameSize / 4);
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
    QString filename = "sende_r" + QString::number(currentFrame.timestamp) + ".jpg";
    if (!rgbImage.save(filename))
    {
        qDebug() << "Failed to save image to";
    }
    else
    {
        qDebug() << "Image saved to";
    }
}

void NetworkSender::getInfoSend()
{
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0)
    {
        int fps = frameCount.load() / elapsedSeconds.count();
        double bandwidth = (totalBytesSent / elapsedSeconds.count()) / 125000.0;
        frameCount = 0;
        totalBytesSent = 0;
        startTime = currentTime;

        if (_callback != nullptr)
            _callback->onRenderInfoSender(currentEncodedFrame.width, currentEncodedFrame.height, fps, bandwidth);
    }
}
