#include "AsyncLogger.h"

AsyncLogger::AsyncLogger() : m_Quitting(false) {}

AsyncLogger::~AsyncLogger()
{
    std::unique_lock<std::mutex> lock(m_StateChange);

    m_Quitting = true;
    m_CVStart.notify_all();

    m_Thread.join();
}

void AsyncLogger::LoggerThreadEntry()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_StateChange);

        m_CVStart.wait(lock, [&]()
                       {
                           return !m_Queue.empty();
                       });

        if (m_Quitting)
            return;

        auto log(std::move(m_Queue.front()));
        m_Queue.pop();

        lock.release();

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
