#pragma once
#include "../stdafx.h"
#include <thread>
#include <condition_variable>
#include "IWorker.h"

class BaseWorker : public IWorker
{
public:
    ~BaseWorker();

    NO_COPY(BaseWorker);
    NO_MOVE(BaseWorker);

    void setSaveCallback(SaveEventHandler callback) override;
    void setFinishCallback(FinishEventHandler callback) override;

    WorkerState getState() const override;

    bool Run(const WorkingConfig &config, std::atomic<size_t> *tick) override;
    void Cancel() override;

protected:
    BaseWorker();

    virtual void Prepare() = 0;

    virtual size_t ProcessOne() = 0;

    std::shared_ptr<Configuration> m_Config;
    LogicLevel m_Logic;

    CancellationToken m_Cancel;

private:
    SaveEventHandler m_EventSave;
    FinishEventHandler m_EventFinish;

    size_t m_SaveInterval;
    size_t m_NotSaved;
    size_t m_Resume;

    WorkerState m_State;

    std::atomic<size_t> *m_Tick;

    std::mutex m_StateChange;
    std::condition_variable m_CVStart;
    std::thread m_Thread;

    void WorkerThreadEntry();

    void ProcessAll();
};
