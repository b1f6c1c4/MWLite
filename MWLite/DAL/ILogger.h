#pragma once
#include "../stdafx.h"
#include "../Entities/Configuration.h"
#include <memory>

struct Logging
{
    Configuration Config;
    LogicLevel Logic;
    std::shared_ptr<std::vector<size_t>> Result;

    Logging(const Configuration &config, LogicLevel level, std::shared_ptr<std::vector<size_t>> result);
    Logging(const Logging &other) = delete;
    Logging(Logging &&other) = default;
    Logging &operator=(const Logging &other) = delete;
    Logging &operator=(Logging &&other) = default;
};

inline Logging::Logging(const Configuration &config, LogicLevel level, std::shared_ptr<std::vector<size_t>> result) : Config(config), Logic(level), Result(result) { }

class ILogger
{
public:
    virtual ~ILogger() { }

    virtual void Log(Logging &&log) = 0;

protected:
    ILogger() { }
};
