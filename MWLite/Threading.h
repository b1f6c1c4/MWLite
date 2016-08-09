#pragma once
#include "stdafx.h"
#include <thread>

void SetThreadName(const char *);
void SetThreadName(std::thread &, const char *);
