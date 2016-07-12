#pragma once
#include "ILogger.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class AsyncLogger : public ILogger
{
public:
    ~AsyncLogger();

    void Log(Logging &&log) override;

protected:
    AsyncLogger();

    virtual void Process(const Logging &log) = 0;

private:
    std::mutex m_StateChange;
    std::condition_variable m_CVStart;
    std::thread m_Thread;
    bool m_Quitting;

    std::queue<Logging> m_Queue;

    void LoggerThreadEntry();
};
