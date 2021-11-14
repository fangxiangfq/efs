#pragma once
#include <string>
#include <memory>
#include <vector>
#include "buffer.h"

namespace Efc
{
    struct pcap_pkthdr
    {
        struct timeval ts;    
        uint32_t caplen;  
        uint32_t len;       
    };

    struct udp_hdr 
    {
        uint16_t src_port;		   
        uint16_t dst_port;		  
        uint16_t udplen;			 
        uint16_t chk_sum;		
    };

    struct packet
    {
        packet(struct timeval& ts, const char* data, size_t len):ts_(ts), data_(data), len_(len){}
        struct timeval ts_;
        const char* data_;
        size_t len_;
    };

    class PcapReader
    {
    public:
        PcapReader(){};
        ~PcapReader();
        bool open(const char* file);
        void parse(bool linuxCooked);

        std::pair<const std::shared_ptr<Buffer::Buffer>&, const std::vector<packet>&> data();
    private:
        void next();
        size_t fileSize_{0};
        size_t curReadSize_{0};
        size_t packNum_{0};
        int fd_{-1};
        bool linuxCooked_{0};
        bool fileOpened_{0};

        std::shared_ptr<Buffer::Buffer> buf_;
        std::vector<packet> packs_;
    };
}