#pragma once

#include <string>

class Connection
{
private:
    int fd_;
    std::string read_buffer;
    std::string write_buffer;

public:
    Connection() : fd_(-1) {}

    Connection(int client_fd) : fd_(client_fd) {}

    void init(int client_fd);

    int fd();

    const char *write_buffer_data();

    size_t write_buffer_len();

    void append_to_read_buffer(char *buffer, size_t len);

    void process();

    void clear_write_buffer();
};
