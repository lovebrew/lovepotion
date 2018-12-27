#include "common/runtime.h"

#include "socket/objects/socket.h"

Socket::Socket(const std::string & type, int sock) : Object(type)
{
    this->connected = false;

    this->timeout.tv_sec = 5;
    this->timeout.tv_usec = 0;

    this->sock = sock;
}

bool Socket::Ready()
{
    int returnValue = 0;
    fd_set mask;

    do
    {
        FD_ZERO(&mask);
        FD_SET(this->sockfd, &mask);

        returnValue = select(this->sockfd + 1, &mask, NULL, NULL, &this->timeout);
    } while (returnValue == EINTR);

    return returnValue == 1;
}

int Socket::GetPeerName(char * origin, int * port)
{
    struct sockaddr_in address;
    socklen_t length = sizeof(address);

    int error = getpeername(this->sockfd, (struct sockaddr *)&address, &length);

    LOG("getpeername: %d", error);

    inet_ntop(AF_INET, &address.sin_addr.s_addr, origin, 0x40);
    *port = htons(address.sin_port);

    return error;
}

int Socket::GetSockName(char * origin, int * port)
{
    struct sockaddr_in address;
    socklen_t length = sizeof(address);

    int error = getsockname(this->sockfd, (struct sockaddr *)&address, &length);

    LOG("getsockname: %d", error);

    inet_ntop(AF_INET, &address.sin_addr.s_addr, origin, 0x40);
    *port = htons(address.sin_port);

    return error;
}

bool Socket::IsConnected()
{
    return this->connected;
}

void Socket::SetTimeout(double timeout)
{
    this->timeout.tv_sec = timeout;
}

int Socket::Close()
{
    shutdown(this->sockfd, SHUT_RDWR);
    
    this->sockfd = -1;
    this->connected = false;

    return 0;
}