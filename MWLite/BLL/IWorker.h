#pragma once
#include "../stdafx.h"
#include "../Entities/Configuration.h"
#include <functional>
#include <atomic>

struct WorkingConfig
{
    std::shared_ptr<Configuration> Config;
    LogicLevel Logic;
    size_t Repetition;
    size_t SaveInterval;

    WorkingConfig(std::shared_ptr<Configuration> config, LogicLevel level, size_t repetition, size_t saveInterval);
};

inline WorkingConfig::WorkingConfig(std::shared_ptr<Configuration> config, LogicLevel level, size_t repetition, size_t saveInterval) : Config(config), Logic(level), Repetition(repetition), SaveInterval(saveInterval) { }

typedef std::function<void(const Configuration &, LogicLevel, std::shared_ptr<std::vector<size_t>>)> SaveEventHandler;
typedef std::function<void()> FinishEventHandler;

enum class WorkerState
{
    Idle = 0,
    Running = 1,
    Cancelling = 2,
    Finished = 3,
    Quitting = 4
};

class IWorker
{
public:
    virtual ~IWorker() { }

    virtual void setSaveCallback(SaveEventHandler callback) = 0;
    virtual void setFinishCallback(FinishEventHandler callback) = 0;

    virtual WorkerState getState() const = 0;

    virtual bool Run(const WorkingConfig &config, std::atomic<size_t> *tick) = 0;
    virtual void Cancel() = 0;

protected:
    IWorker() { }
};
