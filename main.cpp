#include "./WebServer/WebServer.h"

int main()
{
    WebServer server;
    server.start(8080, 8, 60, 65535);

    return 0;
}
