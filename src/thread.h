#pragma once
#include <functional>
#include <string>

class Thread
{
    public:
    typedef std::function<void ()> ThreadFunc;

    explicit Thread(ThreadFunc, const std::string& name = std::string());
    ~Thread();

    void start();
    int join(); 

    bool started() const { return started_; }

    const std::string& name() const { return name_; }

    private:
    void setDefaultName();

    bool       started_;
    bool       joined_;
    ThreadFunc func_;
    std::string     name_;
};