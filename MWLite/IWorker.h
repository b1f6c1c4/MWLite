#pragma once
#include "stdafx.h"
#include "Configuration.h"
#include <functional>

typedef std::function<void(const Configuration &, size_t *, size_t)> SaveEventHandler;
typedef std::function<void()> FinishEventHandler;

enum class WorkerState
{
    Idle,
    Running,
    Cancelling,
    Finished
};

class IWorker
{
public:
    virtual ~IWorker() { }

    virtual void setSaveInterval(int value) = 0;
    virtual int getSaveInterval() const = 0;

    virtual void setSaveCallback(SaveEventHandler callback) = 0;
    virtual void setFinishCallback(FinishEventHandler callback) = 0;

    virtual WorkerState getState() const = 0;

    virtual bool Run(const Configuration &config, size_t repetition) = 0;
    virtual void Cancel() = 0;

protected:
    IWorker() { }
};
