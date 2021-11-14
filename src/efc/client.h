#pragma once
#include <atomic>
#include <string>
#include <vector>
#include <set>
#include "eventsloop.h"
#include "workerfactory.h"
#include "pcap.h"

namespace Efc
{
    class Client
    {
    public:
        Client(Event::EventsLoop* loop, const std::string& nameArg = "efc", uint16_t threadNum = 7);
        void read(const char* pcapfile, bool linuxCooked);
        void setDst(const std::string& ip, const uint16_t& start_port, const uint16_t& interval, const uint16_t& portnum);
        void setLogLevel(level_enum lv) { logger_.setLogLv(lv); }

    private:
        void setThreadData();    
    private:
        Logger logger_;
        Event::EventsLoop* loop_;
        const std::string name_;
        std::unique_ptr<Thread::WorkerFactory> workerFactory_;
        std::atomic<bool> started_{false};
        const uint16_t SendPortStart_{50000};
        PcapReader reader_;

        struct ThreadData
        {
            size_t curpos_;
            std::vector<packet> packs_;
            std::vector<Net::InetAddress> dsts_;
            std::shared_ptr<Buffer::Buffer> buf_;
        };

        using ThreadDataPtr = std::shared_ptr<ThreadData>;

        std::unique_ptr<ThreadDataPtr[]> threadDatas_;
        std::vector<Net::InetAddress> dsts_;
    };
}