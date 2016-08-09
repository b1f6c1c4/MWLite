#pragma once
#include "../stdafx.h"
#include "../../../CppUtil/CppUtil/AsyncWorker.hpp"
#include "ILogger.h"

class Logger : public AsyncWorker<Logging>, public ILogger
{
public:
    Logger();
    ~Logger();

    void Log(Logging &&log) override;

protected:
    void Process(const Logging &log, const ICancellationToken &cancel) override;

private:
    size_t m_Sequence;

    static std::wstring Hash(const Configuration &config);

    void TouchDirectory(const std::wstring &path);
};
