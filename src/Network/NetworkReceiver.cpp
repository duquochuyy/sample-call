#include "./NetworkReceiver.h"

NetworkReceiver::NetworkReceiver(int port)
    : _port(port),
      isRecevingData(false),
      receiver_fd(-1),
      sender_sock(-1),
      totalBytesReceive(0),
      frameCount(0),
      packetCount(0) {
    struct sockaddr_in address;
    int opt = 1;

    // create socket
    if ((receiver_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // specifying the address
    if (setsockopt(receiver_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
        0) {
        perror("setsockopt failed");
        close(receiver_fd);
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    // binding socket
    if (bind(receiver_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // listening
    if (listen(receiver_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cerr << "Start port: " << _port << std::endl;
}

NetworkReceiver::~NetworkReceiver() {
    if (receiver_fd >= 0) {
        close(receiver_fd);
        receiver_fd = -1;
    }
    if (sender_sock >= 0) {
        close(sender_sock);
        sender_sock = -1;
    }
}

void NetworkReceiver::registerCallback(Callback *callback) {
    _callback = callback;
}

void NetworkReceiver::handleRequestConnect() {
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    if ((sender_sock = accept(receiver_fd, (struct sockaddr *)&address,
                              (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        return;
    }
    std::cerr << "Connection accepted" << std::endl;

    // connect back to partner
    int partnerPort;
    recv(sender_sock, &partnerPort, sizeof(partnerPort), 0);
    connectBackThread =
        std::thread(&NetworkReceiver::handleConnectBack, this, partnerPort);
    connectBackThread.detach();

    isRecevingData = true;
    receiveThread = std::thread(&NetworkReceiver::receiveData, this);
    receiveThread.detach();
}

void NetworkReceiver::handleConnectBack(int partnerPort) {
    {
        std::lock_guard<std::mutex> lock(callbackMutex);
        if (_callback) {
            _callback->onAcceptedConnection("127.0.0.1", partnerPort);
        }
    }
}

void NetworkReceiver::handleRequestDisconnect() {
    if (_callback)
        _callback->onRequestDisconnect();
}

void NetworkReceiver::receiveData() {
    uint64_t newestFrameTimestamp = 0;
    startTime = std::chrono::steady_clock::now();
    while (isRecevingData) {
        std::vector<char> buffer(PACKET_SIZE);

        int bytesRead = recv(sender_sock, buffer.data(), buffer.size(), 0);

        packetCount++;

        if (bytesRead <= 0) {
            std::cerr << "Connection closed or error occurred" << std::endl;
            break;
        }
        if (bytesRead > 0) {
            std::string message(buffer.begin(), buffer.begin() + bytesRead);
            if (message == "disconnect") {
                handleRequestDisconnect();
                return;
            }
        }

        totalBytesReceive += bytesRead;

        uint64_t timestamp;
        int totalPackets, packetId, width, height, chunkSize;
        int headerSize = sizeof(uint64_t) + 5 * sizeof(int);

        std::memcpy(&timestamp, buffer.data(), sizeof(uint64_t));
        if (timestamp < newestFrameTimestamp)
            continue;

        std::memcpy(&totalPackets, buffer.data() + sizeof(uint64_t),
                    sizeof(int));

        std::memcpy(&packetId, buffer.data() + sizeof(uint64_t) + sizeof(int),
                    sizeof(int));

        std::memcpy(&width, buffer.data() + sizeof(uint64_t) + 2 * sizeof(int),
                    sizeof(int));

        std::memcpy(&height, buffer.data() + sizeof(uint64_t) + 3 * sizeof(int),
                    sizeof(int));

        std::memcpy(&chunkSize,
                    buffer.data() + sizeof(uint64_t) + 4 * sizeof(int),
                    sizeof(int));

        std::vector<uchar> frameDataChunk(
            buffer.begin() + headerSize,
            buffer.begin() + headerSize + chunkSize);

        bufferFrames[timestamp][packetId] = frameDataChunk;

        if (bufferFrames[timestamp].size() == totalPackets) {
            int fullFrameSize = 0;
            for (const auto &packet : bufferFrames[timestamp]) {
                fullFrameSize += packet.second.size();
            }
            std::vector<uchar> fullFrameData(fullFrameSize);
            int offset = 0;
            for (const auto &packet : bufferFrames[timestamp]) {
                const auto &dataVector = packet.second;
                std::memcpy(fullFrameData.data() + offset, dataVector.data(),
                            dataVector.size());
                offset += dataVector.size();
            }

            // Process the complete frame
            newestFrameTimestamp = timestamp;
            frameCount++;
            getInfo();

            std::cerr << "receive frame " << timestamp << " " << fullFrameSize
                      << " " << totalPackets << std::endl;
            if (_callback) {
                _callback->onReceiveDataFrame(fullFrameData, timestamp);
            }
            for (auto it = std::begin(bufferFrames);
                 it != std::end(bufferFrames);) {
                if (it->first <= newestFrameTimestamp) {
                    it = bufferFrames.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
}

void NetworkReceiver::disconnect() {
    if (isRecevingData) {
        isRecevingData = false;
        if (receiveThread.joinable()) {
            receiveThread.join();
        }
        if (connectBackThread.joinable()) {
            connectBackThread.join();
        }
        if (listenThread.joinable()) {
            listenThread.join();
        }
        if (sender_sock >= 0) {
            close(sender_sock);
            sender_sock = -1;
        }
    }
}

void NetworkReceiver::startListening() {
    isRecevingData = false;
    listenThread = std::thread([this]() { handleRequestConnect(); });
    listenThread.detach();
}

void NetworkReceiver::getInfo() {
    auto currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsedSeconds = currentTime - startTime;
    if (elapsedSeconds.count() >= 1.0) {
        int fps = frameCount.load() / elapsedSeconds.count();
        int pps = packetCount.load() / elapsedSeconds.count();
        double bandwidth =
            std::round((totalBytesReceive / elapsedSeconds.count()) / 125000.0 *
                       1000.0) /
            1000.0;
        frameCount = 0;
        packetCount = 0;
        totalBytesReceive = 0;
        startTime = currentTime;

        if (_callback != nullptr)
            _callback->onShowInfoReceive(fps, pps, bandwidth);
    }
}
