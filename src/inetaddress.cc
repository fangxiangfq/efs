#include "inetaddress.h"

#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
#pragma GCC diagnostic error "-Wold-style-cast"

namespace Socket
{
    InetAddress::InetAddress(uint16_t port) 
    {
        memset(&addr_, 0, sizeof addr_);
        addr_.sin_family = AF_INET;
        in_addr_t ip = kInaddrAny;
        addr_.sin_addr.s_addr = sockets::hostToNetwork32(ip);
        addr_.sin_port = sockets::hostToNetwork16(port);
    }
    
    InetAddress::InetAddress(std::string ip, uint16_t port) 
    :ip_(ip), port_(port)
    {
        memset(&addr_, 0, sizeof addr_);
        addr->sin_family = AF_INET;
        addr->sin_port = hostToNetwork16(port);
        if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
        {
            //todo log
        }
    }
    
    InetAddress::InetAddress(const struct sockaddr_in& addr) 
    :addr_(addr)
    {
        char buf[64] = "";
        port_ = sockets::networkToHost16(addr_->sin_port);
        ::inet_ntop(AF_INET, &addr4->sin_addr, buf, static_cast<socklen_t>(size));
        ip.append(buf);
    }
}