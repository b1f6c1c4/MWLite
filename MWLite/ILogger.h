#pragma once
#include "stdafx.h"
#include "Configuration.h"

class ILogger
{
public:
    virtual ~ILogger() { }

    virtual bool Log(const Configuration &config, size_t *result, size_t length) = 0;

protected:
    ILogger() { }
};
