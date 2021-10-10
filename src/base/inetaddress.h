#pragma once
#include <netinet/in.h>
#include <string>

namespace Net
{
    const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
    struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);
    class InetAddress
    {
    public:
        explicit InetAddress(uint16_t port = 0);
        InetAddress(std::string ip, uint16_t port);
        explicit InetAddress(const struct sockaddr_in& addr);
        const struct sockaddr* getSockAddr() const { return sockaddr_cast(&addr_); }
        void setSockAddr(const struct sockaddr_in& addr){addr_ = addr;}
        uint16_t port() const{ return port_;}
        std::string ip() const{ return ip_;};
    private:
        std::string ip_;
        uint16_t port_;
        struct sockaddr_in addr_;
    };
}

