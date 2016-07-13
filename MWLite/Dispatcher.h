#pragma once
#include "stdafx.h"
#include "./DAL/ILogger.h"
#include "./BLL/IWorker.h"
#include <vector>
#include <queue>
#include <condition_variable>

class Dispatcher
{
public:
    explicit Dispatcher(int numWorkers);
    ~Dispatcher();

    Dispatcher(const Dispatcher &other) = delete;
    Dispatcher(Dispatcher &&other) = delete;
    Dispatcher &operator=(const Dispatcher &other) = delete;
    Dispatcher &operator=(Dispatcher &&other) = delete;

    size_t GetNumWorkers() const;

    void Schedule(const Configuration &config, size_t repetition, size_t saveInterval);

    void CancelWorker(int id);
    WorkerState GetWorkerState(int id) const;
    void EmptyQueue();

private:
    std::mutex m_MtxQueue;
    std::queue<WorkingConfig> m_Queue;

    std::vector<std::unique_ptr<IWorker>> m_Workers;

    ILogger *m_Logger;

    void NotifyAllWorkers();
};
