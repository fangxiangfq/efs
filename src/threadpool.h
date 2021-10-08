#pragma once
#include <functional>
#include <string>
#include "thread.h"
class ThreadPool
{
public:
    typedef std::function<void ()> Task;

    explicit ThreadPool(const std::string& nameArg = std::string("ThreadPool"));
    ~ThreadPool();
    void setThreadInitCallback(const Task& cb)
    { threadInitCallback_ = cb; }

    void start(int numThreads);
    void stop();

    const std::string& name() const
    { return name_; }

    size_t queueSize() const;

    void run(Task f);

private:
    std::string name_;
    Task threadInitCallback_;
    std::vector<std::unique_ptr<Thread>> threads_;
    bool running_;
};