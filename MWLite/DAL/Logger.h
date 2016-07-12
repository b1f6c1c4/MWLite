#pragma once
#include "../stdafx.h"
#include "AsyncLogger.h"
#include <string>
#include <mutex>

class Logger : public AsyncLogger
{
public:
    Logger();
    ~Logger();

protected:
    void Process(const Logging &log) override;

private:
    size_t m_Sequence;

    static std::string Hash(const Configuration &config);

    void TouchDirectory(std::string path);
};
