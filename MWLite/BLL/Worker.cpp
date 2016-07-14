#include "Worker.h"
#include "IGenerator.h"
#include "TotalGenerator.h"
#include "ProbGenerator.h"
#include "MWSolver.h"
#include "SLSolver.h"
#include "DLSolver.h"
#include "../Threading.h"

Worker::Worker() : m_EventSave(nullptr), m_EventFinish(nullptr), m_SaveInterval(1), m_NotSaved(0), m_Resume(0), m_State(WorkerState::Idle), m_Cancel(false) , m_Tick(nullptr), m_Thread(&Worker::WorkerThreadEntry, this) {}

Worker::~Worker()
{
    {
        std::lock_guard<std::mutex> lock(m_StateChange);

        m_State = WorkerState::Quitting;
        m_Cancel = true;
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

bool Worker::Run(const WorkingConfig &config, std::atomic<size_t> *tick)
{
    if (m_State != WorkerState::Idle && m_State != WorkerState::Finished)
        return false;

    m_Tick = tick;
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
        m_Cancel = true;
    }
    m_CVStart.notify_all();
}

void Worker::WorkerThreadEntry()
{
    SetThreadName("Worker");

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

        {
            std::unique_lock<std::mutex> lock(m_StateChange);

            m_State = WorkerState::Finished;
            m_Cancel = false;
            if (m_State == WorkerState::Quitting)
                return;
        }

        m_EventFinish();
    }
}

void Worker::ProcessAll()
{
    auto length = !m_Config.UseTotalMines ? m_Config.Width * m_Config.Height : m_Config.TotalMines + 1;

    auto result = new size_t[length];
    memset(result, 0, sizeof(size_t) * length);

    std::unique_ptr<IGenerator> gen;
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
        if (m_Cancel)
            break;

        gen->GenerateGame(game);

        newSolver();

        auto res = slv->Solve(&m_Cancel);
        if (m_Cancel)
            break;

        if (m_Tick != nullptr)
            ++*m_Tick;

        if (res < 0 || res >= length)
            throw new std::exception("solution outbound");

        result[res]++;

        m_Resume--;
        m_NotSaved++;

        if (m_NotSaved >= m_SaveInterval)
        {
            m_EventSave(m_Config, result, length);

            m_NotSaved = 0;
            memset(result, 0, sizeof(size_t) * length);
        }
    }

    if (m_NotSaved > 0)
        m_EventSave(m_Config, result, length);

    delete[] result;
}
