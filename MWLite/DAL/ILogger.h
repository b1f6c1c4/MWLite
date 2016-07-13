#pragma once
#include "../stdafx.h"
#include "../Entities/Configuration.h"
#include <memory>

struct Logging
{
    const Configuration *Config;
    std::unique_ptr<size_t[]> Result;
    size_t Length;

    Logging(const Configuration &config, const size_t *result, size_t length);
    Logging(const Logging &other) = delete;
    Logging(Logging &&other) = default;
    Logging &operator=(const Logging &other) = delete;
    Logging &operator=(Logging &&other) = default;
};

inline Logging::Logging(const Configuration &config, const size_t *result, size_t length) : Config(&config), Result(std::make_unique<size_t[]>(length)), Length(length)
{
    memcpy(Result.get(), result, sizeof(*result) * length);
}

class ILogger
{
public:
    virtual ~ILogger() { }

    virtual void Log(Logging &&log) = 0;

protected:
    ILogger() { }
};
