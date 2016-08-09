#pragma once
#include "../stdafx.h"
#include "IWorker.h"
#include <thread>
#include <condition_variable>

class Worker : public IWorker
{
public:
    Worker();
    ~Worker();

    Worker(const Worker &) = delete;
    Worker(Worker &&) = delete;
    Worker &operator=(const Worker &) = delete;
    Worker &operator=(Worker &&) = delete;

    void setSaveCallback(SaveEventHandler callback) override;
    void setFinishCallback(FinishEventHandler callback) override;

    WorkerState getState() const override;

    bool Run(const WorkingConfig &config, std::atomic<size_t> *tick) override;
    void Cancel() override;

private:
    SaveEventHandler m_EventSave;
    FinishEventHandler m_EventFinish;

    size_t m_SaveInterval;
    std::shared_ptr<Configuration> m_Config;
    LogicLevel m_Logic;
    size_t m_NotSaved;
    size_t m_Resume;

    WorkerState m_State;
    CancellationToken m_Cancel;

    std::atomic<size_t> *m_Tick;

    std::mutex m_StateChange;
    std::condition_variable m_CVStart;
    std::thread m_Thread;

    void WorkerThreadEntry();

    void ProcessAll();
};
