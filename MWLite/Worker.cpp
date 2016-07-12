#include "Worker.h"

Worker::Worker() : m_SaveInterval(1), m_NotSaved(0), m_Resume(0), m_Result(nullptr), m_ResultLength(0), m_State(WorkerState::Idle), m_Thread(&Worker::WorkerThreadEntry, this) {}

Worker::~Worker()
{
    {
        std::lock_guard<std::mutex> lock(m_StateChange);

        m_State = WorkerState::Quitting;
    }
    m_CVStart.notify_all();

    m_Thread.join();
}

void Worker::setSaveCallback(SaveEventHandler callback)
{
    m_EventSave = callback;
}

void Worker::setFinishCallback(FinishEventHandler callback)
{
    m_EventFinish = callback;
}

WorkerState Worker::getState() const
{
    return m_State;
}

bool Worker::Run(const WorkingConfig &config)
{
    if (m_State != WorkerState::Idle && m_State != WorkerState::Finished)
        return false;

    m_Config = config.Config;
    m_Resume = config.Repetition;
    m_SaveInterval = config.SaveInterval;
    m_NotSaved = 0;
    m_State = WorkerState::Running;
    m_CVStart.notify_all();

    return true;
}

void Worker::Cancel()
{
    if (m_State != WorkerState::Running)
        return;
    {
        std::lock_guard<std::mutex> lock(m_StateChange);

        if (m_State != WorkerState::Running)
            return;

        m_State = WorkerState::Cancelling;
    }
    m_CVStart.notify_all();
}

void Worker::WorkerThreadEntry()
{
    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(m_StateChange);

            m_CVStart.wait(lock, [&]()
                           {
                               return m_State != WorkerState::Idle &&
                                   m_State != WorkerState::Finished;
                           });

            if (m_State == WorkerState::Quitting)
                return;
        }

        while (m_Resume > 0)
        {
            if (m_State == WorkerState::Cancelling || m_State == WorkerState::Quitting)
                break;

            ProcessOne();

            m_Resume--;
            m_NotSaved++;

            if (m_NotSaved >= m_SaveInterval)
            {
                m_EventSave(m_Config, m_Result, m_ResultLength);

                m_NotSaved = 0;
            }
        }
        
        if (m_NotSaved > 0)
            m_EventSave(m_Config, m_Result, m_ResultLength);

        {
            std::unique_lock<std::mutex> lock(m_StateChange);

            m_State = WorkerState::Finished;
            if (m_State == WorkerState::Quitting)
                return;
        }

        m_EventFinish();
    }
}

void Worker::ProcessOne()
{
    // TODO
}
