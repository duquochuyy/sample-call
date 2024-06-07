// utils/utils.cpp
#include "utils.h"

uint64_t Utils::getCurrentTimestamp()
{
    auto now = std::chrono::steady_clock::now();
    auto epoch = now.time_since_epoch();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    return milliseconds.count();
}
