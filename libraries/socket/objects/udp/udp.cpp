#include "common/runtime.h"
#include "socket/objects/udp/udp.h"

#define CLASS_CONN "udp{connected}"
#define CLASS_DISC "udp{unconnected}"

UDP::UDP(int & success) : Socket()
{
    const char * error = this->Create(AF_UNSPEC, SOCK_DGRAM, 0);

    if (error != NULL)
    {
        success = -1;
        return;
    }

    success = IO::IO_DONE;
}

std::string UDP::GetString()
{
    const char * res = (this->isConnected) ? CLASS_CONN : CLASS_DISC;
    char buffer[34 + strlen(res)];

    sprintf(buffer, "%s: %p", res, this);

    return buffer;
}

int UDP::ReceiveFrom(std::vector<char> & buffer, Socket::Address & remote)
{
    sockaddr_storage address;
    socklen_t addressSize = sizeof(address);

    char name[INET_ADDRSTRLEN];
    char port[6];

    int error = 0;

    this->timeout.MarkStart();

    if (this->sockfd == SOCKET_INVALID)
        return IO::IO_CLOSED;

    for (;;)
    {
        long taken = (long)recvfrom(this->sockfd, buffer.data(), buffer.size(), 0, (sockaddr *)&address, &addressSize);

        if (taken > 0)
        {
            buffer.resize(taken);

            error = getnameinfo((sockaddr *)&address, addressSize, name, INET_ADDRSTRLEN, port, 6, NI_NUMERICHOST | NI_NUMERICSERV);

            if (error)
                return error;

            remote.ip = name;
            remote.port = port;

            return IO::IO_DONE;
        }

        error = errno;

        if (taken == 0)
            return IO::IO_CLOSED;
        else if (error == EINTR)
            continue;
        else if (error != EAGAIN)
            return error;
        else if ((error = this->Wait(WAITFD_R, &this->timeout)) != IO::IO_DONE)
            return error;
    }

    return IO::IO_UNKNOWN;
}

int UDP::SendTo(const char * data, size_t length, size_t * sent, Address & address)
{
    int error;
    *sent = 0;

    if (this->sockfd == SOCKET_INVALID)
        return IO::IO_CLOSED;

    addrinfo hint;
    addrinfo * resolved = NULL;

    memset(&hint, 0, sizeof(hint));

    hint.ai_family = this->family;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;

    error = getaddrinfo(address.ip.c_str(), address.port.c_str(), &hint, &resolved);

    if (error)
        return error;

    this->timeout.MarkStart();

    for (;;)
    {
        long put = (long)sendto(this->sockfd, data, length, 0, resolved->ai_addr, resolved->ai_addrlen);

        if (put >= 0)
        {
            *sent = put;
            return IO::IO_DONE;
        }

        error = errno;

        if (error == EPIPE)
            return IO::IO_CLOSED;
        else if (error == EPROTOTYPE)
            continue;
        else if (error == EINTR)
            continue;
        else if ((error == this->Wait(WAITFD_W, &this->timeout)) != IO::IO_DONE)
            return error;
    }

    return IO::IO_UNKNOWN;
}

std::string UDP::SetOption(const std::string & name, int value)
{
    std::array<std::pair<std::string, int>, 2> options =
    {{
        {"dontroute", SO_DONTROUTE},
        {"broadcast", SO_BROADCAST}
    }};

    for (auto option : options)
    {
        if (option.first == name)
        {
            if (setsockopt(this->sockfd, SOL_SOCKET, option.second, (char *)&value, sizeof(value)) < 0)
                return "setsockopt failed";
        }
        else
            return "unsupported option " + name;
    }

    return "";
}

/*
** "Disconnect" a DGRAM Socket
** See: https://github.com/love2d/love/blob/master/src/libraries/luasocket/libluasocket/inet.c#L367
*/
int UDP::TryDisconnect()
{
    sockaddr_in disconnect;

    memset(&disconnect, 0, sizeof(disconnect));

    disconnect.sin_family = AF_UNSPEC;
    disconnect.sin_addr.s_addr = INADDR_ANY;

    this->_Connect((sockaddr *)&disconnect, sizeof(disconnect));

    return this->isConnected = 0;
}
