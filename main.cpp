#include <csignal>
#include <atomic>
#include "./WebServer/WebServer.h"
#include "./Config/Config.h"

std::atomic<bool> g_stop(false);

int main(int argc, char *argv[])
{
    WebServer server;
    Config config;
  
    auto handle_signal = [](int)
    { g_stop = true; };
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    if (config.parse_args(argc, argv))
    {
        exit(1);
    }

    server.start(config, g_stop);

    return 0;
}
