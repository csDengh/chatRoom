#include "chatClient.h"


int main()
{
    chatRoom::ChatClient client_("127.0.0.1", 8000);
    client_.start();
    client_.loop();

    return 0;
}