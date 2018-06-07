#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/tcp/tcp.h"

int TCP::SetOption(const string & option, int value)
{
    int optionValue = 0;

    if (option == "keepalive")
        printf("stubbed");
    else if (option == "linger")
        printf("stubbed");
    else if (option == "reuseaddr")
        printf("stubbed");
    else if (option == "tcp-nodelay")
        printf("stubbed");

    //if (optionValue != 0)
        //setsockopt(this->sockfd, SOL_SOCKET, optionValue, (char *)&enable, sizeof(bool));

    return 0;
}