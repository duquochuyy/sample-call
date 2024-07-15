#include "./NetworkSender.h"

NetworkSender::NetworkSender(int localPort)
    : localPort(localPort),
      sock(-1),
      isSending(false),
      totalBytesSent(0),
      frameCount(0),
      packetCount(0) {}

NetworkSender::~NetworkSender() { disconnect(); }

void NetworkSender::registerCallback(Callback *callback) {
    _callback = callback;
}

bool NetworkSender::handleConnectPartner(std::string ip, int port) {
    struct sockaddr_in serv_addr;
    // creata socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return false;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return false;
    }
    // connect
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return false;
    }
    { send(sock, &localPort, sizeof(localPort), 0); }
    isSending = true;
    startSending();
    return true;
}

void NetworkSender::addNewEncodedFrame(const ZEncodedFrame &encodedFrame) {
    if (encodedFrame.encodedData.size() == 0)
        return;
    const std::lock_guard<std::mutex> lock(encodedFrameMutex);
    currentEncodedFrame = encodedFrame;
    hasNewFrame = true;
}

void NetworkSender::disconnect() {
    if (isSending) {
        std::string disconnectMessage = "disconnect";
        send(sock, disconnectMessage.c_str(), disconnectMessage.size(), 0);
        isSending = false;
        if (sendThread.joinable()) {
            sendThread.join();
        }
        if (sock >= 0) {
            close(sock);
            sock = -1;
        }
    }
}

void NetworkSender::sendData() {
    while (isSending) {
        if (!hasNewFrame) {
            std::this_thread::sleep_for(
                std::chrono::duration<double, std::milli>(1));
            continue;
        }

        const std::lock_guard<std::mutex> lock(encodedFrameMutex);

        int dataSize = currentEncodedFrame.frameSize;
        int headerSize = sizeof(uint64_t) + 5 * sizeof(int);
        int totalPackets = (dataSize + (PACKET_SIZE - 1) - headerSize) /
                           (PACKET_SIZE - headerSize);
        int offset = 0;

        std::cerr << " send frame: " << currentEncodedFrame.timestamp << " "
                  << currentEncodedFrame.frameSize << " " << totalPackets
                  << std::endl;

        auto startPacketTime = std::chrono::steady_clock::now();

        for (int packetId = 0; packetId < totalPackets; ++packetId) {
            int chunkSize = std::min(static_cast<int>(PACKET_SIZE - headerSize),
                                     dataSize - offset);
            std::vector<char> packet(PACKET_SIZE);

            std::memcpy(packet.data(), &currentEncodedFrame.timestamp,
                        sizeof(uint64_t));

            std::memcpy(packet.data() + sizeof(uint64_t), &totalPackets,
                        sizeof(int));

            std::memcpy(packet.data() + sizeof(uint64_t) + sizeof(int),
                        &packetId, sizeof(int));

            std::memcpy(packet.data() + sizeof(uint64_t) + 2 * sizeof(int),
                        &currentEncodedFrame.width, sizeof(int));

            std::memcpy(packet.data() + sizeof(uint64_t) + 3 * sizeof(int),
                        &currentEncodedFrame.height, sizeof(int));

            std::memcpy(packet.data() + sizeof(uint64_t) + 4 * sizeof(int),
                        &chunkSize, sizeof(int));

            std::memcpy(packet.data() + sizeof(uint64_t) + 5 * sizeof(int),
                        currentEncodedFrame.encodedData.data() + offset,
                        chunkSize);

            offset += chunkSize;

            send(sock, packet.data(), packet.size(), 0);

            packetCount++;

            totalBytesSent += packet.size();
        }

        getInfo();
        hasNewFrame = false;
    }
}

void NetworkSender::startSending() {
    startTime = std::chrono::steady_clock::now();
    sendThread = std::thread([this]() { sendData(); });
    sendThread.detach();
}

void NetworkSender::getInfo() {
    frameCount++;
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0) {
        int fps = frameCount.load() / elapsedSeconds.count();
        int pps = packetCount.load() / elapsedSeconds.count();
        double bandwidth =
            std::round((totalBytesSent / elapsedSeconds.count()) / 125000.0 *
                       1000.0) /
            1000.0;
        frameCount = 0;
        packetCount = 0;
        totalBytesSent = 0;
        startTime = currentTime;

        if (_callback != nullptr)
            _callback->onShowInfoSend(fps, pps, bandwidth);
    }
}
