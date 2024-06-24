#ifndef ZROOTFRAME_H
#define ZROOTFRAME_H

#include <vector>
#include <cstdint> // for uint64_t

struct ZRootFrame
{
    std::vector<uchar> nv12Data;
    int width;
    int height;
    uint64_t timestamp;

    // Default constructor
    ZRootFrame() : width(0), height(0), timestamp(0) {}

    // Parameterized constructor from std::vector<uchar>
    ZRootFrame(const std::vector<uchar> &data, int w, int h, uint64_t ts)
        : nv12Data(data), width(w), height(h), timestamp(ts) {}

    // Constructor from raw uchar* data
    ZRootFrame(const uchar *data, int dataSize, int w, int h, uint64_t ts)
        : nv12Data(data, data + dataSize), width(w), height(h), timestamp(ts) {}

    // Copy constructor
    ZRootFrame(const ZRootFrame &other)
        : nv12Data(other.nv12Data), width(other.width), height(other.height), timestamp(other.timestamp) {}

    // Move constructor
    ZRootFrame(ZRootFrame &&other) noexcept
        : nv12Data(std::move(other.nv12Data)), width(other.width), height(other.height), timestamp(other.timestamp)
    {
        other.width = 0;
        other.height = 0;
        other.timestamp = 0;
    }

    // Copy assignment operator
    ZRootFrame &operator=(const ZRootFrame &other)
    {
        if (this != &other)
        {
            nv12Data = other.nv12Data;
            width = other.width;
            height = other.height;
            timestamp = other.timestamp;
        }
        return *this;
    }

    // Move assignment operator
    ZRootFrame &operator=(ZRootFrame &&other) noexcept
    {
        if (this != &other)
        {
            nv12Data = std::move(other.nv12Data);
            width = other.width;
            height = other.height;
            timestamp = other.timestamp;

            other.width = 0;
            other.height = 0;
            other.timestamp = 0;
        }
        return *this;
    }

    // Destructor
    ~ZRootFrame() = default;

    // Method to get the size of the NV12 data
    size_t getDataSize() const
    {
        return nv12Data.size();
    }

    ZRootFrame clone() const
    {
        return *this;
    }
};

#endif // ZROOTFRAME_H
