#include "./NetworkSender.h"

NetworkSender::NetworkSender(int localPort) : localPort(localPort), sock(-1), isSending(false), totalBytesSent(0), frameCount(0)
{
}

NetworkSender::~NetworkSender()
{
    disconnect();
    delete _labelWidth;
    delete _labelHeight;
    delete _labelFPS;
    delete _labelBitrate;
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
        // std::lock_guard<std::mutex> lock(socketMutex);
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

void NetworkSender::setLabelInfoSend(QLabel *width, QLabel *height, QLabel *FPS, QLabel *bitrate)
{
    _labelWidth = width;
    _labelHeight = height;
    _labelFPS = FPS;
    _labelBitrate = bitrate;
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
    auto frameDuration = std::chrono::duration<double, std::milli>(33.3);
    while (isSending)
    {
        if (!hasNewFrame)
        {
            std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(1));
            continue;
        }
        int dataSize = currentFrame.width * currentFrame.height * 3 / 2;
        int totalPackets = (dataSize + (PACKET_SIZE - 1) - sizeof(uint64_t) - 4 * sizeof(int)) / (PACKET_SIZE - sizeof(uint64_t) - 4 * sizeof(int));
        int offset = 0;

        for (int packetId = 0; packetId < totalPackets; ++packetId)
        {
            std::vector<char> packet(PACKET_SIZE);
            // Copy timestamp
            std::memcpy(packet.data(), &currentFrame.timestamp, sizeof(uint64_t));

            // Copy packerId
            std::memcpy(packet.data() + sizeof(uint64_t), &packetId, sizeof(int));

            // Copy totalPackets
            std::memcpy(packet.data() + sizeof(uint64_t) + sizeof(int), &totalPackets, sizeof(int));

            // Copy width
            std::memcpy(packet.data() + sizeof(uint64_t) + 2 * sizeof(int), &currentFrame.width, sizeof(int));

            // Copy height
            std::memcpy(packet.data() + sizeof(uint64_t) + 3 * sizeof(int), &currentFrame.height, sizeof(int));

            // Copy frame data chunk
            int chunkSize = std::min(static_cast<int>(PACKET_SIZE - sizeof(uint64_t) - 4 * sizeof(int)), dataSize - offset);
            std::memcpy(packet.data() + sizeof(uint64_t) + 4 * sizeof(int), currentFrame.yuv420pData + offset, chunkSize);
            offset += chunkSize;

            // Send packet
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

void NetworkSender::renderInfoSend(int width, int height, int FPS, double bitrate)
{
    _labelWidth->setText(QString::number(width));
    _labelHeight->setText(QString::number(height));
    _labelFPS->setText(QString::number(FPS));
    _labelBitrate->setText(QString::number(bitrate));
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

        renderInfoSend(currentFrame.width, currentFrame.height, fps, bandwidth);
    }
}
