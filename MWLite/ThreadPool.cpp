#include "ThreadPool.h"

// the constructor just launches some amount of workers
ThreadPool::ThreadPool(size_t threads) : m_Quitting(false)
{
    for (size_t i = 0; i < threads; ++i)
        m_Workers.emplace_back(&ThreadPool::WorkerThreadEntry, this);
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args ...)>::type>
{
    using return_type = typename std::result_of<F(Args ...)>::type;

    auto task =
        std::make_shared<std::packaged_task<return_type()>>(
                                                            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                                                           );

    std::future<return_type> res = task->get_future();

    {
        std::unique_lock<std::mutex> lock(m_MtxQueue);

        // don't allow enqueueing after stopping the pool
        if (m_Quitting)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        m_Tasks.emplace([task]()
                        {
                            (*task)();
                        });
    }
    m_CVQueue.notify_one();

    return res;
}

void ThreadPool::WorkerThreadEntry()
{
    while (true)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(this->m_MtxQueue);

            this->m_CVQueue.wait(lock,
                                 [this]
                                 {
                                     return this->m_Quitting || !this->m_Tasks.empty();
                                 });

            if (this->m_Quitting && this->m_Tasks.empty())
                return;

            task = std::move(this->m_Tasks.front());

            this->m_Tasks.pop();
        }

        task();
    }
}

// the destructor joins all threads
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_MtxQueue);
        m_Quitting = true;
    }

    m_CVQueue.notify_all();

    for (auto &worker : m_Workers)
        worker.join();
}
