#include "./NetworkSender.h"

NetworkSender::NetworkSender(int localPort) : localPort(localPort), sock(-1), isSending(false)
{
}

NetworkSender::~NetworkSender()
{
    disconnect();
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

void NetworkSender::sendData(const ZVideoFrame &frame)
{
    const std::lock_guard<std::mutex> lock(frameMutex);
    currentFrame = frame;
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

void NetworkSender::startSending()
{
    sendThread = std::thread([this]()
                             {
                                  while (true)
                                  {
                                      while (isSending)
                                      {
                                           std::this_thread::sleep_for(std::chrono::milliseconds(33));
                                           int dataSize = currentFrame.width  * currentFrame.height * 3/2;
                                           int totalPackets = (dataSize + (PACKET_SIZE - 1) - sizeof(uint64_t) - 4 * sizeof(int)) / (PACKET_SIZE - sizeof(uint64_t) -  4 * sizeof(int));
                                           int offset = 0;
                                            // qDebug() << "send data" << currentFrame.timestamp;
                                           for (int packetId = 0; packetId < totalPackets; ++packetId ) {
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
                                            // qDebug() << "send data" << currentFrame.timestamp << packetId << "total" << totalPackets;
                                            send(sock, packet.data(), packet.size(), 0);
                                           }
                                      }
                                  } });
    sendThread.detach();
}

#include <QImage>
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
    // if (!rgbImage.save("testImageSender.jpg"))
    // {
    //     qDebug() << "Failed to save image to";
    // }
    // else
    // {
    //     qDebug() << "Image saved to";
    // }
}
