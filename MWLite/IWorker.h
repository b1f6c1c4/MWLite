#pragma once
#include "stdafx.h"
#include "Configuration.h"
#include <functional>

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

typedef std::function<void(const Configuration &, size_t *, size_t)> SaveEventHandler;
typedef std::function<void()> FinishEventHandler;

enum class WorkerState
{
    Idle,
    Running,
    Cancelling,
    Finished,
    Quitting
};

class IWorker
{
public:
    virtual ~IWorker() { }

    virtual void setSaveCallback(SaveEventHandler callback) = 0;
    virtual void setFinishCallback(FinishEventHandler callback) = 0;

    virtual WorkerState getState() const = 0;

    virtual bool Run(const WorkingConfig &config) = 0;
    virtual void Cancel() = 0;

protected:
    IWorker() { }
};
