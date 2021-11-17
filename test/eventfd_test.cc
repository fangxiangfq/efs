#include "event.h"
#include "logger.h"
/*************************************************************
Conclusion: EventFd Not suitable for transferring data between threads


[2021-11-17 22:10:30.858] [debug] [event.cc:70] write data pointer 0x7fff3352f710
[2021-11-17 22:10:30.858] [debug] [event.cc:70] write data pointer 0x7fff3352f711
[2021-11-17 22:10:30.858] [debug] [event.cc:70] write data pointer 0x7fff3352f712
[2021-11-17 22:10:30.858] [debug] [event.cc:70] write data pointer 0x7fff3352f713
[2021-11-17 22:10:30.858] [debug] [event.cc:70] write data pointer 0x7fff3352f714
[2021-11-17 22:10:30.858] [debug] [event.cc:70] write data pointer 0x7fff3352f715
[2021-11-17 22:10:30.858] [debug] [event.cc:70] write data pointer 0x7fff3352f716
[2021-11-17 22:10:30.858] [debug] [event.cc:70] write data pointer 0x7fff3352f717
[2021-11-17 22:10:30.858] [debug] [event.cc:70] write data pointer 0x7fff3352f718
[2021-11-17 22:10:30.858] [debug] [event.cc:70] write data pointer 0x7fff3352f719
[2021-11-17 22:10:30.858] [debug] [event.cc:50] read data pointer 0x4fff8013da6cd
[2021-11-17 22:10:30.858] [critical] [event.cc:46] eventfd read failed ret[-1] errno[11]
[2021-11-17 22:10:30.858] [critical] [event.cc:46] eventfd read failed ret[-1] errno[11]
[2021-11-17 22:10:30.858] [critical] [event.cc:46] eventfd read failed ret[-1] errno[11]
[2021-11-17 22:10:30.858] [critical] [event.cc:46] eventfd read failed ret[-1] errno[11]
[2021-11-17 22:10:30.858] [critical] [event.cc:46] eventfd read failed ret[-1] errno[11]
[2021-11-17 22:10:30.858] [critical] [event.cc:46] eventfd read failed ret[-1] errno[11]
[2021-11-17 22:10:30.858] [critical] [event.cc:46] eventfd read failed ret[-1] errno[11]
[2021-11-17 22:10:30.858] [critical] [event.cc:46] eventfd read failed ret[-1] errno[11]
[2021-11-17 22:10:30.858] [critical] [event.cc:46] eventfd read failed ret[-1] errno[11]

**************************************************************/
int main()
{
    spdlog::set_level(debug);
    char test[10];
    Event::TaskEvent taskEv;
    for(int i = 0; i < 10; ++i){
        taskEv.write(&test[i]);
    }

    for(int i = 0; i < 10; ++i){
        taskEv.read();
    }
    return 0;
}