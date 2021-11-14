#include "pcap.h"
#include <iostream>
#include "logger.h"

int main(int argc, char** argv)
{
    Logger log;
    Efc::PcapReader pcapReader;
    if(argc < 2)
        pcapReader.open("/home/fanx/videol.pcap");
    else if(!pcapReader.open(argv[1])){
        return -1;
    }

    pcapReader.parse(true);
}