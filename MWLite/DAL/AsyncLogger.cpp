#include "AsyncLogger.h"

AsyncLogger::AsyncLogger() : m_Thread(&AsyncLogger::LoggerThreadEntry, this), m_Quitting(false) {}

AsyncLogger::~AsyncLogger()
{
    std::unique_lock<std::mutex> lock(m_StateChange);

    m_Quitting = true;

    lock.unlock();

    m_CVStart.notify_all();

    m_Thread.join();
}

void AsyncLogger::LoggerThreadEntry()
{
    std::unique_lock<std::mutex> lock(m_StateChange, std::defer_lock);
    while (true)
    {
        // enter lock
        lock.lock();

        m_CVStart.wait(lock, [&]()
                       {
                           return m_Quitting || !m_Queue.empty();
                       });

        if (m_Quitting)
            return;

        auto log(std::move(m_Queue.front()));
        m_Queue.pop();

        // exit lock
        lock.unlock();

        Process(log);
    }
}

void AsyncLogger::Log(Logging &&log)
{
    {
        std::lock_guard<std::mutex> lock(m_StateChange);

        m_Queue.emplace(std::move(log));
    }

    m_CVStart.notify_all();
}
