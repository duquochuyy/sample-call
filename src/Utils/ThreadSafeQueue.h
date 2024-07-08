#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class ThreadSafeQueue {
   public:
    ThreadSafeQueue() {}
    void push(const T &value);

    void push(T &&value);

    bool tryPop(T &value);

    void waitPop(T &value);

    void pop();

    bool empty() const;

    int size() const;

   private:
    mutable std::mutex mutex_;  // cho phép thay đổi trong luồng có const
    std::queue<T> queue_;
    std::condition_variable condVar_;
};
#include "./ThreadSafeQueue.cpp"

#endif