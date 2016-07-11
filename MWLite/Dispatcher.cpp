#include "Dispatcher.h"
#include "Worker.h"
#include "Logger.h"

Dispatcher::Dispatcher(int numWorkers) : m_LoggerPool(1), m_Logger(std::unique_ptr<ILogger>(std::make_unique<Logger>()))
{
    for (auto i = 0; i < numWorkers; i++)
    {
        auto worker = new Worker();

        worker->setSaveCallback([this](const Configuration &config, size_t *result, size_t length)
                                {
                                    m_LoggerPool.enqueue(std::bind(&ILogger::Log, m_Logger), config, result, length);
                                });

        worker->setFinishCallback([this, worker]()
                                  {
                                      std::unique_lock<std::mutex> lock(m_MtxQueue);

                                      if (m_Queue.empty())
                                          return;

                                      worker->Run(m_Queue.front());
                                      m_Queue.pop();
                                  });

        // DO NOT write emplace_back(worker) here
        // because emplace_back may throw an exception
        m_Workers.emplace_back(std::unique_ptr<IWorker>(worker));
    }
}

Dispatcher::~Dispatcher()
{
    // TODO
}

void Dispatcher::Schedule(const Configuration &config, size_t repetition, size_t saveInterval)
{
    {
        std::unique_lock<std::mutex> lock(m_MtxQueue);

        m_Queue.emplace(config, repetition, saveInterval);
    }

    NotifyAllWorkers();
}

void Dispatcher::NotifyAllWorkers()
{
    std::unique_lock<std::mutex> lock(m_MtxQueue);

    while (!m_Queue.empty())
    {
        auto flag = true;
        for (auto &worker : m_Workers)
            if (worker->Run(m_Queue.front()))
            {
                flag = false;
                break;
            }

        if (flag)
            return;

        m_Queue.pop();
    }
}
