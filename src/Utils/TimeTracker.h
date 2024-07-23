#ifndef TIMETRACKER_H
#define TIMETRACKER_H

#include <chrono>
#include <mutex>

class TimeTracker {
   public:
    TimeTracker();
    ~TimeTracker();
    void start();
    void stop();
    double getAverageTime();
    void distanceTime(uint64_t from, uint64_t to);

   private:
    double totalTime;
    int callCount;
    std::chrono::steady_clock::time_point startPoint;
    std::chrono::steady_clock::time_point startTime;
    std::mutex mtx;
};

#endif
