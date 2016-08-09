#include "stdafx.h"
#include "Dispatcher.h"
#include <string>

Dispatcher *TheDispatcher = nullptr;

std::wstring WorkingDirectory(L".");

extern "C"
{
    DLL_API void SetWorkingDirectory(const wchar_t *path)
    {
        WorkingDirectory = std::wstring(path);
    }

    DLL_API void CreateWorkers(int numWorkers)
    {
        if (TheDispatcher != nullptr)
        {
            delete TheDispatcher;
            TheDispatcher = nullptr;
        }

        TheDispatcher = new Dispatcher(numWorkers);
    }

    DLL_API size_t GetNumWorkers()
    {
        return TheDispatcher->GetNumWorkers();
    }

    DLL_API void Schedule(Configuration config, size_t repetition, size_t saveInterval)
    {
        TheDispatcher->Schedule(std::make_shared<Configuration>(config), repetition, saveInterval);
    }

    DLL_API void CancelWorker(int id)
    {
        TheDispatcher->CancelWorker(id);
    }

    DLL_API WorkerState GetWorkerState(int id)
    {
        return TheDispatcher->GetWorkerState(id);
    }

    DLL_API void EmptyQueue()
    {
        TheDispatcher->EmptyQueue();
    }

    DLL_API size_t ResetCounter()
    {
        return TheDispatcher->ResetCounter();
    }

    DLL_API void RemoveWorkers()
    {
        if (TheDispatcher != nullptr)
        {
            delete TheDispatcher;
            TheDispatcher = nullptr;
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        RemoveWorkers();
        break;
    }
    return TRUE;
}
