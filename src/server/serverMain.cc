#include "chatServer.h"
#include <signal.h>
#include "chatService.h"
#include <iostream>

void sigIntHandler(int)
{
    chatRoom::ChatService::getInstance()->reset();
    exit(0);
}



int main(int argc, char* argv[])
{
    if(argc < 3)
    {
        std::cerr << "command invalid" <<std::endl;
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);

    signal(SIGINT, sigIntHandler);

    EventLoop loop;
    InetAddress inetaddr(ip, port);
    chatRoom::ChatServer cs(&loop, inetaddr, "hello");
    cs.setThreadNum(4);
    cs.start();
    loop.loop(); 

    return 0;
}