#include "common/runtime.h"

#include "socket/objects/socket.h"

Socket::Socket(const std::string & type, int sock) : Object(type)
{
    this->connected = false;
    this->timeout = 5;
    this->sock = sock;
}

Socket::~Socket()
{
    this->Close();
}

int Socket::Ready(int wait_type)
{
    int returnValue = 0;
    struct pollfd pfd;

    pfd.fd = this->sockfd;
    pfd.events = wait_type;
    pfd.revents = 0;

    if (this->timeout == 0)
        return 0;

    do
    {
        int time = this->timeout >= 0 ? this->timeout : -1;
        returnValue = poll(&pfd, 1, time);
    } while (returnValue == -1 && errno == EINTR);

    return returnValue;
}

int Socket::GetPeerName(char * origin, int * port)
{
    struct sockaddr_in address;
    socklen_t length = sizeof(address);

    int error = getpeername(this->sockfd, (struct sockaddr *)&address, &length);

    inet_ntop(AF_INET, &address.sin_addr.s_addr, origin, 0x40);
    *port = htons(address.sin_port);

    return error;
}

int Socket::GetSockName(char * origin, int * port)
{
    struct sockaddr_in address;
    socklen_t length = sizeof(address);

    int error = getsockname(this->sockfd, (struct sockaddr *)&address, &length);

    inet_ntop(AF_INET, &address.sin_addr.s_addr, origin, 0x40);
    *port = htons(address.sin_port);

    return error;
}

bool Socket::IsConnected()
{
    return this->connected;
}

void Socket::SetBlocking(bool block)
{
    int flags = fcntl(this->sockfd, F_GETFL, 0);

    if (!block) //set it to nonblocking
        fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK);
    else //blocking
        fcntl(this->sockfd, F_SETFL, flags & ~O_NONBLOCK);
}

void Socket::SetTimeout(int timeout)
{
    this->timeout = timeout;
}

int Socket::Close()
{
    shutdown(this->sockfd, SHUT_RDWR);
    close(this->sockfd);
    
    this->sockfd = -1;
    this->connected = false;

    return 0;
}
