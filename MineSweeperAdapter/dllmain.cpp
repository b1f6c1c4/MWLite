#include "stdafx.h"
#include <windows.h>

#include "Worker.h"

extern "C"
{
    ADAPTER_DLL_API Worker *MakeWorker(Configuration config, LogicLevel level, size_t repetition)
    {
        auto worker = new Worker;
        worker->Config = std::make_shared<Configuration>(config);
        worker->Logic = level;
        worker->Repetition = repetition;

        return worker;
    }

    ADAPTER_DLL_API size_t *Run(Worker *worker, size_t &len)
    {
        worker->Process();

        len = worker->Result.size();
        auto res = new size_t[len];
        memcpy(res, &*worker->Result.begin(), len * sizeof(size_t));
        return res;
    }

    ADAPTER_DLL_API void CancelWorker(Worker *ptr)
    {
        ptr->Cancel();
    }

    ADAPTER_DLL_API void DisposeWorker(Worker *ptr)
    {
        if (ptr != nullptr)
            delete[] ptr;
    }

    ADAPTER_DLL_API void DisposeResult(size_t *ptr)
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
