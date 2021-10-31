#include "efs.h"
#include "server.h"

int main(int argc, char** argv)
{
    Event::EventsLoop loop;
    Server::Server serv(&loop);
    serv.start();
    return 0;
}