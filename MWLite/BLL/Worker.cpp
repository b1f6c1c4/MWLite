#include "Worker.h"
#include "IGenerator.h"
#include "TotalGenerator.h"
#include "ProbGenerator.h"
#include "MWSolver.h"
#include "SLSolver.h"
#include "DLSolver.h"

Worker::Worker() : m_SaveInterval(1), m_NotSaved(0), m_Resume(0), m_Result(nullptr), m_ResultLength(0), m_State(WorkerState::Idle), m_Thread(&Worker::WorkerThreadEntry, this) {}

Worker::~Worker()
{
    {
        std::lock_guard<std::mutex> lock(m_StateChange);

        m_State = WorkerState::Quitting;
    }
    m_CVStart.notify_all();

    m_Thread.join();

    if (m_Result != nullptr)
    {
        delete [] m_Result;
        m_Result = nullptr;
    }
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

        ProcessAll();
        
        if (m_NotSaved > 0)
            m_EventSave(m_Config, m_Result, m_ResultLength);

        {
            std::unique_lock<std::mutex> lock(m_StateChange);

            m_State = WorkerState::Finished;
            if (m_State == WorkerState::Quitting)
                return;
        }

        m_EventFinish();
        
        if (m_Result != nullptr)
        {
            delete[] m_Result;
            m_Result = nullptr;
        }
    }
}

void Worker::ProcessAll()
{
    m_ResultLength = m_Config.UseTotalMines ? m_Config.Width * m_Config.Height : m_Config.TotalMines;

    m_Result = new size_t[m_ResultLength];
    memset(m_Result, 0, sizeof(size_t) * m_ResultLength);

    std::unique_ptr<IGenerator> gen(nullptr);
    if (m_Config.UseTotalMines)
        gen = std::make_unique<TotalGenerator>(m_Config.Width, m_Config.Height, m_Config.TotalMines);
    else
        gen = std::make_unique<ProbGenerator>(m_Config.Width, m_Config.Height, m_Config.Probability);

    Game game(0, 0);

    std::unique_ptr<MWSolver> slv(nullptr);
    auto newSolver = [&]()
    {
        if (m_Config.DisableDual)
            slv = std::make_unique<SLSolver>(game);
        else
            slv = std::make_unique<DLSolver>(game);
    };

    while (m_Resume > 0)
    {
        if (m_State == WorkerState::Cancelling || m_State == WorkerState::Quitting)
            break;

        gen->GenerateGame(game);
        newSolver();
        auto res = slv->Solve();
        m_Result[res]++;

        m_Resume--;
        m_NotSaved++;

        if (m_NotSaved >= m_SaveInterval)
        {
            m_EventSave(m_Config, m_Result, m_ResultLength);

            m_NotSaved = 0;
            memset(m_Result, 0, sizeof(size_t) * m_ResultLength);
        }
    }
}