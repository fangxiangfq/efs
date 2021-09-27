#pragma once
#include <netinet/in.h>
#include <string>

namespace Socket
{
    class InetAddress
    {
    public:
        explicit InetAddress(uint16_t port = 0);
        InetAddress(std::string ip, uint16_t port);
        explicit InetAddress(const struct sockaddr_in& addr);
        const struct sockaddr_in* getSockAddr() const { return &addr_; }
        void setSockAddr(const struct sockaddr_in& addr){addr_ = addr;}
        uint16_t port() const{ return port_;}
        std::string ip() const{ return ip_;};
    private:
        uint16_t port_;
        std::string ip_;
        struct sockaddr_in addr_;
    };
}

