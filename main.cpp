#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    // create socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // bind
    struct sockaddr_in server_addr;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    bind(listen_fd, (sockaddr *)&server_addr, sizeof(server_addr));

    // listen
    listen(listen_fd, 10);

    std::cout << "Server is listening..." << std::endl;

    for (;;)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // accept
        int conn_fd = accept(listen_fd, (sockaddr *)&client_addr, &client_len);
        std::cout << "new client connected!" << std::endl;
        char buffer[60] = {0};

        // recv
        ssize_t buf_len = recv(conn_fd, buffer, sizeof(buffer), 0);

        // send
        if (buf_len > 0)
        {
            std::cout << "receive: " << buffer;
            send(conn_fd, buffer, buf_len, 0);
        }
        close(conn_fd);
    }
    close(listen_fd);

    return 0;
}