#pragma once
#include "stdafx.h"
#include "Configuration.h"
#include <functional>

struct WorkingConfig
{
    Configuration Configuration;
    size_t Repetition;
    size_t SaveInterval;
};

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
