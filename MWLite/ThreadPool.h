#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

class ThreadPool
{
public:
    explicit ThreadPool(size_t);
    ~ThreadPool();

    template <class F, class... Args>
    auto enqueue(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args ...)>::type>;

private:
    // the entry point
    void WorkerThreadEntry();

    // need to keep track of threads so we can join them
    std::vector<std::thread> m_Workers;

    // the task queue
    std::queue<std::function<void()>> m_Tasks;

    // synchronization
    std::mutex m_MtxQueue;
    std::condition_variable m_CVQueue;
    bool m_Quitting;
};
