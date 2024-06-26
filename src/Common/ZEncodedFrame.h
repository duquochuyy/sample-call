#ifndef ZENCODEDFRAME_H
#define ZENCODEDFRAME_H

#include <cstdint>
#include <vector>

#include "./ZVideoFrame.h"

struct ZEncodedFrame {
    std::vector<uchar> encodedData;
    int frameSize;
    int width;
    int height;
    uint64_t timestamp;

    // Constructor mặc định
    ZEncodedFrame() : frameSize(0), width(0), height(0), timestamp(0) {}

    // Constructor với đầy đủ thông tin
    ZEncodedFrame(const uchar *data, int _frameSize, int _width, int _height,
                  uint64_t _timestamp)
        : encodedData(data, data + _frameSize),
          frameSize(_frameSize),
          width(_width),
          height(_height),
          timestamp(_timestamp) {}

    ZEncodedFrame(const std::vector<uchar> &_encodedData, int _frameSize,
                  int _width, int _height, uint64_t _timestamp)
        : encodedData(_encodedData),
          frameSize(_frameSize),
          width(_width),
          height(_height),
          timestamp(_timestamp) {}

    ZEncodedFrame(std::vector<uchar> &&_encodedData, int _frameSize, int _width,
                  int _height, uint64_t _timestamp)
        : encodedData(std::move(_encodedData)),
          frameSize(_frameSize),
          width(_width),
          height(_height),
          timestamp(_timestamp) {}

    // Copy constructor
    ZEncodedFrame(const ZEncodedFrame &other)
        : encodedData(other.encodedData),
          frameSize(other.frameSize),
          width(other.width),
          height(other.height),
          timestamp(other.timestamp) {}

    // Move constructor
    ZEncodedFrame(ZEncodedFrame &&other) noexcept
        : encodedData(std::move(other.encodedData)),
          frameSize(other.frameSize),
          width(other.width),
          height(other.height),
          timestamp(other.timestamp) {}

    // Assignment operator
    ZEncodedFrame &operator=(const ZEncodedFrame &other) {
        if (this != &other) {
            encodedData = other.encodedData;
            frameSize = other.frameSize;
            width = other.width;
            height = other.height;
            timestamp = other.timestamp;
        }
        return *this;
    }

    // Move assignment operator
    ZEncodedFrame &operator=(ZEncodedFrame &&other) noexcept {
        if (this != &other) {
            encodedData = std::move(other.encodedData);
            frameSize = other.frameSize;
            width = other.width;
            height = other.height;
            timestamp = other.timestamp;
        }
        return *this;
    }
    ~ZEncodedFrame() = default;
};

#endif