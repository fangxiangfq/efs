#include "inetaddress.h"
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#pragma GCC diagnostic ignored "-Wold-style-cast"
static const in_addr_t kInaddrAny = INADDR_ANY;
#pragma GCC diagnostic error "-Wold-style-cast"

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

namespace Socket
{
    const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr)
    {
        return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
    }

    struct sockaddr* sockaddr_cast(struct sockaddr_in* addr)
    {
        return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
    }

    InetAddress::InetAddress(uint16_t port) 
    {
        memset(&addr_, 0, sizeof addr_);
        addr_.sin_family = AF_INET;
        in_addr_t ip = kInaddrAny;
        addr_.sin_addr.s_addr = htobe32(ip);
        addr_.sin_port = htobe16(port);
    }
    
    InetAddress::InetAddress(std::string ip, uint16_t port) 
    :ip_(ip), port_(port)
    {
        memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_port = htobe16(port);
        if (::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr) <= 0)
        {
            //todo log
        }
    }
    
    InetAddress::InetAddress(const struct sockaddr_in& addr) 
    :ip_(), addr_(addr) 
    {
        char buf[64] = "";
        port_ = htobe16(addr_.sin_port);
        ::inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(sizeof buf));
        ip_.append(buf);
    }
}