#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/tcp/tcp.h"

TCP::TCP() : Socket(SOCK_STREAM) {}

TCP::TCP(int sockfd) : Socket(SOCK_STREAM, sockfd) {}

int TCP::Accept()
{
    struct sockaddr_in fromAddress = {0};
    socklen_t addressLength;

    listen(this->sockfd, 1);
    int newSocket = accept(this->sockfd,  (struct sockaddr *)&fromAddress, &addressLength);

    return newSocket;
}

int TCP::Listen()
{
    return 0;
}

void TCP::SetSockfd(int sockfd)
{
    this->sockfd = sockfd;
}

int TCP::GetPort()
{
    return this->port;
}

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