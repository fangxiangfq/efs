#include "base/eventsloop.h"
#include "base/threadpool.h"
//todo as a base class sup http tcp udp
namespace Server
{
    class Server
    {
    public:
        enum Option
        {
            kNoReusePort,
            kReusePort,
        };

        Event::EventsLoop* getLoop() const { return loop_; }

        void setThreadNum(int numThreads);
        void setThreadInitCallback(const Thread::ThreadInitCallback& cb)   { threadInitCallback_ = cb; }

        void start();

    private:
    };
}