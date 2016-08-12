#pragma once
#include "../stdafx.h"

class BaseWorker
{
public:
    virtual ~BaseWorker();

    size_t Repetition;

    std::vector<size_t> Result;

    void Process();

    void Cancel();

protected:
    BaseWorker();

    CancellationToken m_Cancel;

    virtual void Prepare();
    virtual size_t ProcessOne() = 0;
};
