#include "pcap.h"
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include "logger.h"

const size_t PCAP_HEADER_SIZE = 24;
const size_t PCAP_PACKET_HEADER_SIZE = 16;
const uint32_t PCAP_MAGIC_COOKIE = 0xa1b2c3d4;

namespace Efc
{
    PcapReader::~PcapReader() 
    {
        if(fd_ > 0)
            close(fd_);
    }

    bool PcapReader::open(const char* file) 
    {
        int saveErrno = 0;
        fd_ = ::open(file, O_RDONLY);
        if(fd_ < 0){
            STD_ERROR("read file[{}] fail errno[{}]", file, errno);
            return false;
        }
        struct stat buf;
        if(0 > fstat(fd_, &buf)){
            STD_ERROR("stat file[{}] fail errno[{}]", file, errno);
            return false;
        }

        fileSize_ = buf.st_size;
        buf_ = std::make_shared<Buffer::Buffer>(fileSize_);
        if(!buf_){
            STD_ERROR("alloc buf fail file size[{}]", fileSize_);
            return false;
        }

        ssize_t n = buf_->fread(fd_, &saveErrno, PCAP_HEADER_SIZE);
        if(n < 0 || static_cast<size_t>(n) != PCAP_HEADER_SIZE){
            STD_ERROR("read file len[{}] fail errno[{}]", PCAP_HEADER_SIZE, saveErrno);
            return false;
        }

        curReadSize_ += PCAP_HEADER_SIZE;

        uint32_t cookie = buf_->read<uint32_t>();
        buf_->retrieve(20);
        if(cookie != PCAP_MAGIC_COOKIE){
            STD_ERROR("pcap magic cookie {} dismatch with reserved {}", cookie, PCAP_MAGIC_COOKIE);
            return false;
        }

        fileOpened_ = true;
        return true;
    }
    
    void PcapReader::parse(bool linuxCooked) 
    {
        assert(fileOpened_);
        int saveErrno = 0;
        linuxCooked_ = linuxCooked;
        size_t needReadLen = fileSize_ - curReadSize_;

        ssize_t n = buf_->fread(fd_, &saveErrno, needReadLen);
        if(n < 0 || static_cast<size_t>(n) != needReadLen){
            STD_ERROR("read file len[{}] fail errno[{}]", needReadLen, saveErrno);
        }

        while(curReadSize_ < fileSize_){
            next();
        }
    }

    void PcapReader::next() 
    {
        pcap_pkthdr pkthdr;// host byte order
        pkthdr.ts.tv_sec = buf_->read<uint32_t>();
        pkthdr.ts.tv_usec = buf_->read<uint32_t>();
        pkthdr.caplen = buf_->read<uint32_t>();
        pkthdr.len = buf_->read<uint32_t>();
        uint32_t udpoffset = 34;
        if(linuxCooked_){
            buf_->retrieve(2);
            udpoffset += 2;
        }
        buf_->retrieve(34);
        udp_hdr udphdr;// network byte order
        udphdr.src_port = buf_->nread<uint16_t>();
        udphdr.dst_port = buf_->nread<uint16_t>();
        udphdr.udplen = buf_->nread<uint16_t>();
        udphdr.chk_sum = buf_->nread<uint16_t>();

        packNum_++;

        if(udphdr.udplen != pkthdr.caplen - udpoffset || udphdr.udplen < 8){
            if(pkthdr.caplen < 64 && udphdr.udplen < pkthdr.caplen - udpoffset){
                buf_->retrieve(64 - pkthdr.caplen);
                curReadSize_ += 64 - pkthdr.caplen;
            }
            else{
                STD_ERROR("packnum[{}] parse udp head fail size[{}] udplen[{}] filesize[{}] curpos[{}]", packNum_, pkthdr.caplen, udphdr.udplen, fileSize_, curReadSize_);
                abort();
            }
        }
        curReadSize_ += PCAP_PACKET_HEADER_SIZE;
        curReadSize_ += pkthdr.caplen;
        
        packs_.emplace_back(pkthdr.ts, buf_->peek(), udphdr.udplen - 8);

        buf_->retrieve(udphdr.udplen - 8);
    }

    std::pair<const std::shared_ptr<Buffer::Buffer>&, const std::vector<packet>&> PcapReader::data()
    {
        return std::make_pair<const std::shared_ptr<Buffer::Buffer>&, const std::vector<packet>&>(buf_, packs_);
    }
}