#include "Logger.h"

std::string Logger::level_to_str(Level level)
{
    switch(level)
    {
    case Level::DEBUG:
    {
        return "[DEBUG]";
    }
    case Level::INFO:
    {
        return "[INFO]";
    }
    case Level::WARN:
    {
        return "[WARN]";
    }
    case Level::ERROR:
    {
        return "[ERROR]";
    }
    case Level::FATAL:
    {
        return "[FATAL]";
    }
    default:
    {
        return "[UNKNOWN]";
    }
    }
}

int Logger::init(const std::string& filename, Level level)
{
    file.open(filename, std::ios::app);

    if(!file.is_open())
    {
        std::cout << "open " << filename << " failed.\n";
        return -1;
    }

    log_level = level;
    return 0;
}

void Logger::log(Level level, const std::string &msg)
{
    if (level < log_level)
    {
        return;
    }

    file << level_to_str(level) << " " << msg << "\n";
    file.flush();
}

void Logger::close()
{
    file.close();
}
