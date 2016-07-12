#pragma once
#include "stdafx.h"
#include "./DAL/ILogger.h"
#include "./BLL/IWorker.h"
#include <vector>
#include <queue>
#include <condition_variable>

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

    ILogger *m_Logger;

    void NotifyAllWorkers();
};
