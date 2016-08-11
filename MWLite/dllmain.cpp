#include "stdafx.h"
#include <windows.h>

#include "BLL/Worker.h"

extern "C"
{
    DLL_API size_t *Run(Configuration config, LogicLevel level, size_t repetition, size_t &len)
    {
        Worker worker;
        worker.Config = std::make_shared<Configuration>(config);
        worker.Logic = level;
        worker.Repetition = repetition;

        worker.Process();

        len = worker.Result.size();
        auto res = new size_t[len];
        memcpy(res, &*worker.Result.begin(), len * sizeof(size_t));
        return res;
    }

    DLL_API void Dispose(size_t *ptr)
    {
        if (ptr != nullptr)
            delete[] ptr;
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
