#include "common/runtime.h"
#include "socket/objects/socket/socket.h"

// void Socket::SetTimeout(int duration)
// {
//     this->timeout = duration;
// }

// int Socket::Ready(int waitType)
// {
//     int returnValue = 0;
//     struct pollfd pfd;

//     pfd.fd = this->sockfd;
//     pfd.events = waitType;
//     pfd.revents = 0;

//     if (this->timeout == 0)
//         return 0;

//     do
//     {
//         int time = this->timeout >= 0 ? this->timeout : -1;
//         returnValue = poll(&pfd, 1, time);
//     } while (returnValue == -1 && errno == EINTR);

//     return returnValue;
// }

int Socket::GetPeerName(Socket::Address & out)
{
    struct sockaddr_in address;
    socklen_t length = sizeof(address);

    int error = getpeername(this->sockfd, (struct sockaddr *)&address, &length);
    char buffer[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &address.sin_addr.s_addr, buffer, INET_ADDRSTRLEN);

    out.ip = buffer;
    out.port = htons(address.sin_port);

    return error;
}

int Socket::GetSockName(Socket::Address & out)
{
    struct sockaddr_in address;
    socklen_t length = sizeof(address);

    int error = getsockname(this->sockfd, (struct sockaddr *)&address, &length);
    char buffer[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &address.sin_addr.s_addr, buffer, INET_ADDRSTRLEN);

    out.ip = buffer;
    out.port = htons(address.sin_port);

    return error;
}

void Socket::SetBlocking(bool shouldBlock)
{
    int flags = fcntl(this->sockfd, F_GETFL, 0);

    if (!shouldBlock) //set it to nonblocking
        fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK);
    else //blocking
        fcntl(this->sockfd, F_SETFL, flags & ~O_NONBLOCK);
}

int Socket::TryBind(const Socket::Address & host)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;

    addrinfo * resolved;

    int result = getaddrinfo(host.ip.c_str(), host.port.c_str(), &hints, &resolved);

    if (result != 0)
    {
        if (resolved)
            freeaddrinfo(resolved);

        return result;
    }

    for (auto item = resolved; item; item = item->ai_next)
    {
        if (this->sockfd == SOCKET_INVALID)
        {
            this->Destroy();
            this->Create(item->ai_socktype, item->ai_family, item->ai_protocol);
            this->SetBlocking(false);
        }

        result = bind(this->sockfd, item->ai_addr, item->ai_addrlen);

        if (result == 0)
            break;
    }

    return 1;
}

int Socket::TryConnect(const Socket::Address & peer)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;

    addrinfo * resolved;

    int result = getaddrinfo(peer.ip.c_str(), peer.port.c_str(), &hints, &resolved);

    if (result != 0)
    {
        if (resolved)
            freeaddrinfo(resolved);

        return result;
    }

    for (auto item = resolved; item; item = item->ai_next)
    {
        this->timeout.start = this->timeout.GetTime();

        if (this->sockfd == SOCKET_INVALID)
        {
            this->Destroy();
            this->Create(item->ai_socktype, item->ai_family, item->ai_protocol);
            this->SetBlocking(false);
        }

        result = connect(this->sockfd, item->ai_addr, item->ai_addrlen);

        if (result == 0)
            break;
    }

    return this->isConnected = 1;
}

int Socket::Create(int type, int domain, int protocol)
{
    this->sockfd = socket(type, domain, protocol);

    if (this->sockfd != SOCKET_INVALID)
        return IO_DONE;

    return errno;
}

void Socket::Destroy()
{
    if (this->sockfd == SOCKET_INVALID)
        return;

    close(this->sockfd);
    this->sockfd = SOCKET_INVALID;
}

void Socket::Close()
{
    shutdown(this->sockfd, SHUT_RDWR);
    close(this->sockfd);
}
