#pragma once

#include <fstream>
#include <iostream>
#include <string>

enum class Level
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

class Logger
{
private:
    std::ofstream file;
    Level log_level;

    std::string level_to_str(Level level);

public:
    int init(const std::string &filename, Level level);
    void log(Level level, const std::string &msg);
    void close();
};
