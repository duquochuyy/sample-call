#include "./NetworkReceiver.h"

NetworkReceiver::NetworkReceiver(int port) : _port(port), receiver_fd(-1), sender_sock(-1)
{
    struct sockaddr_in address;
    int opt = 1;

    // create socket
    if ((receiver_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // specifying the address
    if (setsockopt(receiver_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        close(receiver_fd);
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    // binding socket
    if (bind(receiver_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // listening
    if (listen(receiver_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    qDebug() << "Start port: " << _port;
}

NetworkReceiver::~NetworkReceiver()
{
    if (receiver_fd >= 0)
    {
        close(receiver_fd);
        receiver_fd = -1;
    }
    if (sender_sock >= 0)
    {
        close(sender_sock);
        sender_sock = -1;
    }
}

void NetworkReceiver::registerCallback(Callback *callback)
{
    _callback = callback;
}

void NetworkReceiver::handleRequestConnect()
{
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    if ((sender_sock = accept(receiver_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        return;
    }
    qDebug() << "Connection accepted";

    // connect back to partner
    int partnerPort;
    //    recv(sender_sock, &partnerPort, sizeof(partnerPort), 0);
    //    connectBackThread = std::thread(&NetworkReceiver::handleConnectBack, this, partnerPort);
    //    connectBackThread.detach();

    receiveThread = std::thread(&NetworkReceiver::receiveData, this);
    receiveThread.detach();
}

void NetworkReceiver::handleConnectBack(int partnerPort)
{
    {
        std::lock_guard<std::mutex> lock(callbackMutex);
        if (_callback)
        {
            _callback->onAcceptedConnection("127.0.0.1", partnerPort);
        }
    }
}

void NetworkReceiver::receiveData()
{
    while (true)
    {
        std::vector<char> buffer(PACKER_SIZE);
        int bytesRead = recv(sender_sock, buffer.data(), buffer.size(), 0);
        if (bytesRead <= 0)
        {
            qDebug() << "Connection closed or error occurred";
            break;
        }

        // Extract timestamp
        uint64_t timestamp;
        std::memcpy(&timestamp, buffer.data(), sizeof(uint64_t));

        // Extract packerId
        int packetId;
        std::memcpy(&packetId, buffer.data() + sizeof(uint64_t), sizeof(int));

        // Extract totalPackets
        int totalPackets;
        std::memcpy(&totalPackets, buffer.data() + sizeof(uint64_t) + sizeof(int), sizeof(int));

        qDebug() << "receive data" << timestamp << packetId << "total" << totalPackets;
        // Extract frame data chunk
        int headerSize = sizeof(uint64_t) + 2 * sizeof(int);
        std::vector<uchar> frameDataChunk(buffer.begin() + headerSize, buffer.end());

        // Store the frame data chunk
        bufferFrames[timestamp][packetId] = frameDataChunk;

        // Check if we have received all packets for this frame
        if (bufferFrames[timestamp].size() == totalPackets)
        {
            // Concatenate all chunks to form the complete frame data
            int dataSize = 0;
            for (const auto &packet : bufferFrames[timestamp])
            {
                dataSize += packet.second.size();
            }

            std::vector<uchar> fullFrameData(dataSize);
            int offset = 0;
            for (const auto &packet : bufferFrames[timestamp])
            {
                std::memcpy(fullFrameData.data() + offset, packet.second.data(), packet.second.size());
                offset += packet.second.size();
            }

            // Process the complete frame
            ZVideoFrame frame(fullFrameData.data(), 640, 480, timestamp);
            qDebug() << "receive frame" << timestamp;
            if (_callback)
            {
                _callback->onReceiveFrame(frame);
            }

            // Remove the processed frame from the map
            bufferFrames.erase(timestamp);
        }
    }
}

void NetworkReceiver::disconnect()
{

}

void NetworkReceiver::startListening()
{
    listenThread = std::thread([this]()
                               {
        while (true) {
            handleRequestConnect();
        } });
    listenThread.detach();
}

#include <QImage>
void NetworkReceiver::testShowImage(const uchar *yuv420pData, int width, int height)
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
    if (!rgbImage.save("testImageReceiver.jpg"))
    {
        qDebug() << "Failed to save image to";
    }
    else
    {
        qDebug() << "Image saved to";
    }
}
