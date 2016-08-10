#include "Dispatcher.h"
#include "./DAL/Logger.h"

Dispatcher::Dispatcher(int numWorkers, std::function<std::shared_ptr<IWorker>()> creator) : m_Logger(std::make_unique<Logger>()), m_Counter(0)
{
    for (auto i = 0; i < numWorkers; i++)
    {
        auto worker = creator();

        worker->setSaveCallback([this](std::shared_ptr<Configuration> config, LogicLevel level, std::shared_ptr<std::vector<size_t>> result)
                                {
                                    m_Logger->Log(Logging(config, level, result));
                                });

        worker->setFinishCallback([this, &worker]()
                                  {
                                      std::unique_lock<std::mutex> lock(m_MtxQueue);

                                      if (m_Queue.empty())
                                          return;

                                      worker->Run(m_Queue.front(), &m_Counter);
                                      m_Queue.pop();
                                  });

        m_Workers.push_back(worker);
    }
}

Dispatcher::~Dispatcher() { }

size_t Dispatcher::GetNumWorkers() const
{
    return m_Workers.size();
}

void Dispatcher::Schedule(std::shared_ptr<Configuration> config, LogicLevel level, size_t repetition, size_t saveInterval)
{
    auto numSave = repetition / saveInterval;
    size_t numSubTasks;

    if (numSave <= 1)
        numSubTasks = 1;
    else if (numSave >= m_Workers.size() * m_Workers.size())
        numSubTasks = m_Workers.size() * m_Workers.size();
    else
        numSubTasks = m_Workers.size();

    auto repeat = repetition / numSubTasks;

    {
        std::unique_lock<std::mutex> lock(m_MtxQueue);

        while (repetition > 0)
        {
            if (repetition >= repeat)
            {
                m_Queue.emplace(config, level, repeat, saveInterval);
                repetition -= repeat;
            }
            else
            {
                m_Queue.emplace(config, level, repetition, saveInterval);
                repetition = 0;
            }
        }
    }

    NotifyAllWorkers();
}

void Dispatcher::CancelWorker(int id)
{
    m_Workers[id]->Cancel();
}

WorkerState Dispatcher::GetWorkerState(int id) const
{
    return m_Workers[id]->getState();
}

void Dispatcher::EmptyQueue()
{
    std::lock_guard<std::mutex> lock(m_MtxQueue);

    std::queue<WorkingConfig> q;
    m_Queue.swap(q);
}

size_t Dispatcher::ResetCounter()
{
    return m_Counter.exchange(0);
}

void Dispatcher::NotifyAllWorkers()
{
    std::unique_lock<std::mutex> lock(m_MtxQueue);

    while (!m_Queue.empty())
    {
        auto flag = true;
        for (auto &worker : m_Workers)
            if (worker->Run(m_Queue.front(), &m_Counter))
            {
                flag = false;
                break;
            }

        if (flag)
            return;

        m_Queue.pop();
    }
}
