#pragma once
#include "../stdafx.h"
#include "AsyncLogger.h"

class Logger : public AsyncLogger
{
public:
    Logger();
    ~Logger();

protected:
    void Process(const Logging &log) override;

private:
    size_t m_Sequence;

    static std::wstring Hash(const Configuration &config);

    void TouchDirectory(const std::wstring &path);
};
