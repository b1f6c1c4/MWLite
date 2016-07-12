#include "Logger.h"
#include <filesystem>
#include <time.h>
#include <sstream>
#include <fstream>
#include <iomanip>

Logger::Logger() : m_Sequence(0) {}

Logger::~Logger() {}

void Logger::Process(const Logging &log)
{
    auto &&theTime = time(nullptr);
    tm now;
    localtime_s(&now, &theTime);

    std::stringstream ss;

    ss << "./db";
    TouchDirectory(ss.str());

    ss << '/' << Hash(*log.Config);
    TouchDirectory(ss.str());

    ss << '/';
    ss << std::setfill('0');
    ss << std::setw(4) << now.tm_year + 1900 << '-';
    ss << std::setw(2) << now.tm_mon + 1 << '-';
    ss << std::setw(2) << now.tm_mday;
    ss << ' ';
    ss << std::setw(2) << now.tm_hour << '-';
    ss << std::setw(2) << now.tm_min << '-';
    ss << std::setw(2) << now.tm_sec << ' ';
    ss << std::setw(8) << std::hex << m_Sequence << ".log";

    std::ofstream fout(ss.str());

    for (auto i = 0; i < log.Length; ++i)
        fout << log.Result[i] << std::endl;

    fout.close();

    m_Sequence++;
}

std::string Logger::Hash(const Configuration &config)
{
    std::stringstream ss;

    if (config.DisableDual)
        ss << "SL";
    else
        ss << "DL";

    ss << ' ';

    ss << config.Width << '-';
    ss << config.Height << '-';

    if (config.UseTotalMines)
        ss << 'T' << config.TotalMines;
    else
        ss << 'P' << std::setprecision(16) << config.Probability;

    return ss.str();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void Logger::TouchDirectory(const std::string &path)
{
    if (std::experimental::filesystem::is_directory(path))
        return;

    if (!std::experimental::filesystem::create_directory(path))
        throw;
}
