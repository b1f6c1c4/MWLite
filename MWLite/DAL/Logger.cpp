#include "Logger.h"
#include <filesystem>
#include <time.h>
#include <sstream>
#include <fstream>
#include <iomanip>

extern std::wstring WorkingDirectory;

Logger::Logger() : m_Sequence(0) {}

Logger::~Logger() {}

void Logger::Process(const Logging &log)
{
    auto &&theTime = time(nullptr);
    tm now;
    localtime_s(&now, &theTime);

    std::wstringstream ss;

    ss << WorkingDirectory << L"db";
    TouchDirectory(ss.str());

    ss << L"/" << Hash(log.Config);
    TouchDirectory(ss.str());

    ss << L"/";
    ss << std::setfill(L'0');
    ss << std::setw(4) << now.tm_year + 1900 << L"-";
    ss << std::setw(2) << now.tm_mon + 1 << L"-";
    ss << std::setw(2) << now.tm_mday;
    ss << L" ";
    ss << std::setw(2) << now.tm_hour << L"-";
    ss << std::setw(2) << now.tm_min << L"-";
    ss << std::setw(2) << now.tm_sec << L" ";
    ss << std::setw(8) << std::hex << m_Sequence << L".log";

    std::ofstream fout(ss.str());

    for (size_t i = 0; i < log.Length; ++i)
        fout << log.Result[i] << std::endl;

    fout.close();

    m_Sequence++;
}

std::wstring Logger::Hash(const Configuration &config)
{
    std::wstringstream ss;

    if (config.DisableDual)
        ss << L"SL";
    else
        ss << L"DL";

    ss << L" ";

    ss << config.Width << L"-";
    ss << config.Height << L"-";

    if (config.UseTotalMines)
        ss << L"T" << config.TotalMines;
    else
        ss << L"P" << std::setprecision(16) << config.Probability;

    return ss.str();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void Logger::TouchDirectory(const std::wstring &path)
{
    if (std::experimental::filesystem::is_directory(path))
        return;

    if (!std::experimental::filesystem::create_directory(path))
        throw;
}
