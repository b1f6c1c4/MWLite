#pragma once
#include "stdafx.h"
#include "ILogger.h"
#include "IWorker.h"
#include "ThreadPool.h"
#include <vector>

class Dispatcher
{
    // TODO
public:
    explicit Dispatcher(int numWorkers);
    ~Dispatcher();

    void Schedule(const Configuration &config, size_t repetition, size_t saveInterval);

private:
    std::mutex m_MtxQueue;
    std::queue<WorkingConfig> m_Queue;

    std::vector<std::unique_ptr<IWorker>> m_Workers;

    ThreadPool m_LoggerPool;
    std::unique_ptr<ILogger> m_Logger;

    void NotifyAllWorkers();
};
