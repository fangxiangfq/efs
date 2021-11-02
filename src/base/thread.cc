#include "thread.h"
#include <pthread.h>

using namespace Event;
namespace Thread
{
    Thread::Thread(Event::TaskEvPtr workerEv, const ThreadInitCallback& cb,
                      const std::string& name) 
    :loop_(NULL),
    exiting_(false),
    thread_(),
    mutex_(),
    cond_(),
    callback_(cb),
    name_(name),
    workerEv_(workerEv)
    {
        
    }
    
    Thread::~Thread() 
    {
        exiting_ = true;
        if (loop_ != NULL) 
        {
            loop_->quit();
            if(thread_->joinable())
                thread_->join();
        }
    }
    
    EventsLoop* Thread::startLoop() 
    {
        thread_.reset(new std::thread(std::bind(&Thread::threadFunc, this)));
        pthread_setname_np(thread_->native_handle(), name_.c_str());
        EventsLoop* loop = NULL;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cond_.wait(lock, [this](){return loop_ != NULL;});
            loop = loop_;
        }

        return loop;
    }
    
    void Thread::threadFunc() 
    {
        EventsLoop loop;

        if (callback_)
        {
            callback_(&loop);
        }

        {
            std::lock_guard<std::mutex> lock(mutex_);
            loop_ = &loop;
            workerEv_->setLoop(loop_);
            workerEv_->enableRead();
            cond_.notify_one();
        }

        loop.loop();
        std::lock_guard<std::mutex> lock(mutex_);
        loop_ = NULL;
    }
}