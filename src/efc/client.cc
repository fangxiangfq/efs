#include "client.h"

namespace Efc
{
    Client::Client(Event::EventsLoop* loop, const std::string& nameArg, uint16_t threadNum) 
    :loop_(loop),
    name_(nameArg),
    workerFactory_(new Thread::WorkerFactory(loop, name_, threadNum)),
    threadDatas_(new ThreadDataPtr[threadNum + 1])
    {
        setLogLevel(debug);
        STD_DEBUG("client construct ok, threadnum {}", threadNum);
    }
    
    void Client::read(const char* pcapfile, bool linuxCooked) 
    {
        assert(reader_.open(pcapfile));
        reader_.parse(linuxCooked);
    }
    
    void Client::setDst(const std::string& ip, const uint16_t& start_port, const uint16_t& interval, const uint16_t& portnum) 
    {
        for(uint32_t i = 0; i < portnum; ++i){
            dsts_.emplace_back(ip, start_port + i*interval);
        }
    }
    
    void Client::setThreadData() 
    {
        auto data = reader_.data();
        for(uint16_t i = 0; i < threadNum_; ++i){
            
        }
    }
}