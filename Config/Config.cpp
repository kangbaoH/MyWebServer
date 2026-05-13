#include "Config.h"

Level Config::parse_log_level(const std::string &level)
{
    if (level == "DEBUG")
    {
        return Level::DEBUG;
    }
    else if (level == "INFO")
    {
        return Level::INFO;
    }
    else if (level == "WARN")
    {
        return Level::WARN;
    }
    else if (level == "ERROR")
    {
        return Level::ERROR;
    }
    else if (level == "FATAL")
    {
        return Level::FATAL;
    }

    std::cerr << "Invalid log level: " << level << "\n";
    exit(1);
}

void Config::print_usage()
{
    std::cerr << "Usage:\n";
    std::cerr << "  ./server [options]\n\n";
    std::cerr << "Options:\n";
    std::cerr << "  --port <port>\n";
    std::cerr << "  --thread-nums <num>\n";
    std::cerr << "  --timeout <seconds>\n";
    std::cerr << "  --max-conn <num>\n";
    std::cerr << "  --log-dir <dir>\n";
    std::cerr << "  --log-level <DEBUG|INFO|WARN|ERROR|FATAL>\n";
    std::cerr << "  --log-size <bytes>\n";
}

bool Config::parse_args(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--port") == 0)
        {
            if (i + 1 >= argc)
            {
                std::cerr << "--port requires a value\n";
                print_usage();
                return 1;
            }

            port = std::atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--thread-nums") == 0)
        {
            if (i + 1 >= argc)
            {
                std::cerr << "--thread-nums requires a value\n";
                print_usage();
                return 1;
            }

            thread_nums = std::atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--timeout") == 0)
        {
            if (i + 1 >= argc)
            {
                std::cerr << "--timeout requires a value\n";
                print_usage();
                return 1;
            }

            timeout = std::atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--max-conn") == 0)
        {
            if (i + 1 >= argc)
            {
                std::cerr << "--max-conn requires a value\n";
                print_usage();
                return 1;
            }

            max_conn = std::atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--log-dir") == 0)
        {
            if (i + 1 >= argc)
            {
                std::cerr << "--log-dir requires a value\n";
                print_usage();
                return 1;
            }

            log_dir = argv[++i];
        }
        else if (strcmp(argv[i], "--log-level") == 0)
        {
            if (i + 1 >= argc)
            {
                std::cerr << "--log-level requires a value\n";
                print_usage();
                return 1;
            }

            log_level = parse_log_level(argv[++i]);
        }
        else if (strcmp(argv[i], "--log-size") == 0)
        {
            if (i + 1 >= argc)
            {
                std::cerr << "--log-size requires a value\n";
                print_usage();
                return 1;
            }

            log_size = static_cast<size_t>(std::atoll(argv[++i]));
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            print_usage();
            return 0;
        }
        else
        {
            std::cerr << "Unknown parameter: " << argv[i] << "\n";
            print_usage();
            return 1;
        }
    }
    return 0;
}
