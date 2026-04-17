// epoll + ThreadPool

#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <vector>
#include "./ThreadPool/ThreadPool.h"
#include "./Connection/Connection.h"

#define MAX_EVENT_NUM 1024

int main()
{
    std::vector<Connection> connections(65535);
    //create epoll
    int epoll_fd = epoll_create1(0);

    // create socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // bind
    struct sockaddr_in server_addr;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    bind(listen_fd, (sockaddr *)&server_addr, sizeof(server_addr));

    // listen
    listen(listen_fd, 1024);

    //set to non-blocking
    int old_flag = fcntl(listen_fd, F_GETFL, 0);
    fcntl(listen_fd, F_SETFL, old_flag | O_NONBLOCK);

    //add to epoll
        //create an epoll event
    epoll_event listen_event;
    listen_event.data.fd = listen_fd;
    listen_event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &listen_event);
    std::vector<epoll_event> events(MAX_EVENT_NUM);

    std::cout << "Server is listening on Port 8080..." << std::endl;

    // ThreadPool
    int notify_fd = eventfd(0, EFD_NONBLOCK);
    ThreadPool pool(8, notify_fd);

    epoll_event notify_event;
    notify_event.data.fd = notify_fd;
    notify_event.events = EPOLLIN;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, notify_fd, &notify_event);

    for (;;)
    {
        int request_num = epoll_wait(epoll_fd, events.data(), MAX_EVENT_NUM, -1);

        for (int i = 0; i < request_num; i += 1)
        {
            epoll_event curr_event = events[i];    //current event

            if (curr_event.data.fd == listen_fd)   // accept new connection                     
            {   
                epoll_event conn_event;

                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);

                for (;;)
                {
                    int conn_fd = accept(listen_fd, (sockaddr *)&client_addr, &client_len);

                    if (conn_fd != -1)
                    {
                        int old_flag = fcntl(conn_fd, F_GETFL, 0);
                        fcntl(conn_fd, F_SETFL, old_flag | O_NONBLOCK);

                        conn_event.data.fd = conn_fd;
                        conn_event.events = EPOLLIN | EPOLLONESHOT | EPOLLET;

                        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &conn_event);

                        connections[conn_fd].init(conn_fd);

                        std::cout << "new client connected!" << std::endl;
                    }
                    else if (conn_fd == -1)
                    {                 
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            break;
                        }
                        else
                        {
                            std::cout << "client connected error!" << std::endl;
                            break;
                        }
                    }
                }
            }
            else if (curr_event.data.fd == notify_fd)  // get processed result
            {                                          // and send to client
                uint64_t result_num;
                if ((read(curr_event.data.fd, &result_num, sizeof(result_num))) == -1)
                {
                    if (errno != EAGAIN || errno != EWOULDBLOCK)
                    {    
                        std::cout << "Failed to get processed data!" << std::endl;
                        result_num = 0;
                    }
                }

                for (uint64_t i = 0; i < result_num; i += 1)
                {
                    Connection *result_connection;
                    {                      
                        std::unique_lock<std::mutex> lock(pool.result_mutex);
                        if(pool.result_queue.empty())
                            break;
                        result_connection = pool.result_queue.front();
                        pool.result_queue.pop();
                    }

                    send(result_connection->fd(),
                         result_connection->write_buffer_data(),
                         result_connection->write_buffer_len(), 0);

                    result_connection->clear_write_buffer();

                    // persistent connection
                    epoll_event rearm_conn_event;
                    rearm_conn_event.events = EPOLLIN | EPOLLONESHOT | EPOLLET;
                    rearm_conn_event.data.fd = result_connection->fd();
                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, result_connection->fd(),
                              &rearm_conn_event);
                }
            }
            else if (curr_event.events & EPOLLIN)    // read data and hand to worker thread
            {
                char buffer[1024];

                ssize_t read_bytes;
                for (;;)
                {
                    read_bytes = read(curr_event.data.fd, (void *)buffer, sizeof(buffer) - 1);

                    if (read_bytes > 0)
                    {
                        connections[curr_event.data.fd].append_to_read_buffer(buffer, read_bytes);
                    }
                    else if (read_bytes == 0)
                    {
                        // 资源释放待添加
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, curr_event.data.fd, NULL);
                        close(curr_event.data.fd);
                        std::cout << "Connection closed by foreign host." << std::endl;
                        break;
                    }
                    else if (read_bytes < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            pool.enqueue(&connections[curr_event.data.fd]);
                            break;
                        }
                        else
                        {
                            std::cout << "read error" << std::endl;
                            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, curr_event.data.fd, NULL);
                            close(curr_event.data.fd);
                            break;
                        }
                    }
                }
            }
        }
    }

    close(listen_fd);
    close(epoll_fd);

    return 0;
}
