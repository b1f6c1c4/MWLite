#pragma once
#include "stdafx.h"
#include "IWorker.h"
#include <thread>
#include <mutex>

class Worker : public IWorker
{
public:
    explicit Worker(const Configuration &config);
    ~Worker();

    void setSaveInterval(int value) override;
    int getSaveInterval() const override;

    void setSaveCallback(SaveEventHandler callback) override;
    void setFinishCallback(FinishEventHandler callback) override;

    WorkerState getState() const override;

    bool Run(const Configuration &config, size_t repetition) override;
    void Cancel() override;

private:
    SaveEventHandler m_EventSave;
    FinishEventHandler m_EventFinish;

    size_t m_SaveInterval;
    Configuration m_Config;
    size_t m_NotSaved;
    size_t m_Resume;

    size_t *m_Result;
    size_t m_ResultLength;

    WorkerState m_State;

    std::mutex m_StateChange;
    std::condition_variable m_CVStart;
    std::thread m_Thread;

    void WorkerThreadEntry();

    void ProcessOne();
};
