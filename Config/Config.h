#pragma once

#include <iostream>
#include <string>
#include <string.h>
#include "../Logger/Logger.h"

class Config
{
public:
    int port = 8080;
    int thread_nums = 8;
    int timeout = 60;
    int max_conn = 65535;
    std::string log_dir = "Logs";
    Level log_level = Level::DEBUG;
    int log_size = 10 * 1024 * 1024;

    Level parse_log_level(const std::string &level);
    void print_usage();
    bool parse_args(int argc, char *argv[]);
};
