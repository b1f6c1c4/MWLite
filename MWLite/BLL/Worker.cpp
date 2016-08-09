#include "Worker.h"
#include "IGenerator.h"
#include "TotalGenerator.h"
#include "ProbGenerator.h"
#include "NotRigorousGenerator.h"
#include "Simulator.h"
#include "../Threading.h"
#include "Strategies/SolverBuilder.h"

Worker::Worker() : m_EventSave(nullptr), m_EventFinish(nullptr), m_SaveInterval(1), m_Logic(LogicLevel::ZeroLogic), m_NotSaved(0), m_Resume(0), m_State(WorkerState::Idle), m_Tick(nullptr), m_Thread(&Worker::WorkerThreadEntry, this) {}

Worker::~Worker()
{
    {
        std::lock_guard<std::mutex> lock(m_StateChange);

        m_State = WorkerState::Quitting;
        m_Cancel.Cancel();
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
    m_Logic = config.Logic;
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
        m_Cancel.Cancel();
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
            m_Cancel.Reset();
            if (m_State == WorkerState::Quitting)
                return;
        }

        m_EventFinish();
    }
}

void Worker::ProcessAll()
{
    auto length = !m_Config->UseTotalMines ? m_Config->Width * m_Config->Height : m_Config->TotalMines + 1;

    auto result = new size_t[length];
    memset(result, 0, sizeof(size_t) * length);

    std::shared_ptr<IGenerator> gen;
    if (m_Config->UseTotalMines)
        gen = std::make_shared<TotalGenerator>(m_Config->Width, m_Config->Height, m_Config->TotalMines);
    else
        gen = std::make_shared<ProbGenerator>(m_Config->Width, m_Config->Height, m_Config->Probability);
    if (m_Config->NotRigorous)
        gen = std::make_shared<NotRigorousGenerator>(gen);

    Game game;
    game.Config = m_Config;

    std::shared_ptr<ISolver> slv(nullptr);
    std::shared_ptr<Simulator> sim(nullptr);

    while (m_Resume > 0)
    {
        if (m_Cancel.IsCancelled())
            break;

        gen->GenerateGame(game);

        slv = SolverBuilder::Instance().Build(m_Logic);
        sim = std::make_shared<Simulator>(game, slv);

        auto res = sim->Solve(m_Cancel, [&](int initial)
                              {
                                  gen->AdjustGame(game, initial);
                              });

        if (m_Cancel.IsCancelled())
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
            m_EventSave(*m_Config, m_Logic, result, length);

            m_NotSaved = 0;
            memset(result, 0, sizeof(size_t) * length);
        }
    }

    if (m_NotSaved > 0)
        m_EventSave(*m_Config, m_Logic, result, length);

    delete[] result;
}
