#include "./TimeTracker.h"

TimeTracker::TimeTracker()
    : totalTime(0), callCount(0), startTime(std::chrono::steady_clock::now()) {}

TimeTracker::~TimeTracker() {}

void TimeTracker::start() { startPoint = std::chrono::steady_clock::now(); }

void TimeTracker::stop() {
    auto end = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(mtx);
    totalTime +=
        std::chrono::duration<double, std::milli>(end - startPoint).count();
    callCount++;
}

double TimeTracker::getAverageTime() {
    std::lock_guard<std::mutex> lock(mtx);
    auto now = std::chrono::steady_clock::now();
    double elapsedSeconds =
        std::chrono::duration<double>(now - startTime).count();
    if (elapsedSeconds >= 1.0) {
        double averageTime = (callCount > 0) ? (totalTime / callCount) : 0.0;
        // Reset tracker sau 1 giây
        totalTime = 0;
        callCount = 0;
        startTime = now;
        return std::floor(averageTime);
    }
    return -1;
}

void TimeTracker::distanceTime(uint64_t from, uint64_t to) {
    std::lock_guard<std::mutex> lock(mtx);
    totalTime += std::chrono::duration<double, std::milli>(to - from).count();
    callCount++;
}
