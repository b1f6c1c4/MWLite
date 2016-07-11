#pragma once
#include "stdafx.h"
#include "ILogger.h"
#include <string>
#include <mutex>

class Logger : public ILogger
{
public:
    Logger();
    ~Logger();

    bool Log(const Configuration &config, size_t *result, size_t length) override;

private:
    std::mutex m_FileIO;
    size_t m_Sequence;

    static std::string Hash(const Configuration &config);

    void TouchDirectory(std::string path);
};
