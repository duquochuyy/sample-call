#include "./ThreadSafeQueue.h"

template <typename T>
void ThreadSafeQueue<T>::push(const T &value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(value);
    condVar_.notify_one();
}

template <typename T>
void ThreadSafeQueue<T>::push(T &&value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(std::move(value));
    condVar_.notify_one();
}

template <typename T>
bool ThreadSafeQueue<T>::tryPop(T &value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty())
    {
        return false;
    }
    value = std::move(queue_.front());
    queue_.pop();
    return true;
}

template <typename T>
void ThreadSafeQueue<T>::waitPop(T &value)
{
    std::unique_lock<std::mutex> lock(mutex_);
    condVar_.wait(lock, [this]
                  { return !queue_.empty(); });
    value = std::move(queue_.front());
    queue_.pop();
}

template <typename T>
void ThreadSafeQueue<T>::pop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.pop();
}

template <typename T>
bool ThreadSafeQueue<T>::empty() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}

template <typename T>
int ThreadSafeQueue<T>::size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
}