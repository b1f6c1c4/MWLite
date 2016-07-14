#pragma once
#include "../stdafx.h"
#include "../Entities/Configuration.h"
#include <functional>
#include <atomic>

struct WorkingConfig
{
    Configuration Config;
    size_t Repetition;
    size_t SaveInterval;

    WorkingConfig(const Configuration &config, size_t repetition, size_t saveInterval);
    WorkingConfig(Configuration &&config, size_t repetition, size_t saveInterval);
};

inline WorkingConfig::WorkingConfig(const Configuration &config, size_t repetition, size_t saveInterval) : Config(config), Repetition(repetition), SaveInterval(saveInterval) { }

inline WorkingConfig::WorkingConfig(Configuration &&config, size_t repetition, size_t saveInterval) : Config(config), Repetition(repetition), SaveInterval(saveInterval) { }

typedef std::function<void(const Configuration &, const size_t *, size_t)> SaveEventHandler;
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
