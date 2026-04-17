#include "Connection.h"

void Connection::init(int client_fd)
{
    fd_ = client_fd;
}

int Connection::fd()
{
    return fd_;
}

const char *Connection::write_buffer_data()
{
    return write_buffer.data();
}

size_t Connection::write_buffer_len()
{
    return write_buffer.length();
}

void Connection::append_to_read_buffer(char *buffer, size_t len)
{
    read_buffer.append(buffer, len);
}

void Connection::process()
{
    if (read_buffer.find("\r\n\r\n") != std::string::npos)
    {
        std::string body = "<html><head><title>My Server</title></head>";
        body += "<body style='text-align:center; margin-top:50px;'>";
        body += "<h1>Hello World!</h1>";
        body += "<h3>Welcome to my C++ Web Server!</h3>";
        body += "</body></html>";

        // 3. 严格按照 HTTP 协议拼装“响应报文”，放入写缓冲区
        write_buffer = "HTTP/1.1 200 OK\r\n";                                        // 状态行
        write_buffer += "Content-Type: text/html\r\n";                               // 告诉浏览器这是 HTML
        write_buffer += "Content-Length: " + std::to_string(body.length()) + "\r\n"; // 身体长度
        write_buffer += "Connection: keep-alive\r\n";                                // 保持长连接
        write_buffer += "\r\n";                                                      // 极其重要的分隔空行！

        write_buffer += body;

        read_buffer.clear();
    }

}

void Connection::clear_write_buffer()
{
    write_buffer.clear();
}
