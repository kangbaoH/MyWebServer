#pragma once

#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <vector>
#include <sys/eventfd.h>
#include <unistd.h>
#include <iostream>
#include "../Connection/Connection.h"

struct Task
{
    Connection *connection;
    int version;

    Task(Connection *conn, int ver);
};

class ThreadPool
{
public:
    std::vector<std::thread> workers;
    std::condition_variable condition;
    std::queue<Task> task_queue;
    std::queue<Connection*> result_queue;
    std::mutex queue_mutex;
    std::mutex result_mutex;
    int notify_fd;
    bool stop;
    
    void init(int threads_num, int event_fd);
    void enqueue(Task new_task);
    ~ThreadPool();
};
