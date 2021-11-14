#include "pcap.h"
#include <iostream>
#include "logger.h"

int main(int argc, char** argv)
{
    Logger log;
    Efc::PcapReader pcapReader;
    if(argc < 2)
        pcapReader.Open("/home/fanx/videol.pcap");
    else if(!pcapReader.Open(argv[1])){
        return -1;
    }

    pcapReader.Parse(true);
}