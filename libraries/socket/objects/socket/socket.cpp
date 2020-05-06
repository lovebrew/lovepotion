#include "common/runtime.h"
#include "socket/objects/socket/socket.h"

int Socket::Wait(int type)
{
    pollfd pfd;

    pfd.fd = this->sockfd;
    pfd.events = type;
    pfd.revents = 0;

    int result;

    if (this->timeout.block == 0.0)
        return IO_TIMEOUT;

    do
    {
        int t = (int)(this->timeout.GetRetry() * 1e3);
        result = poll(&pfd, 1, t >= 0 ? t : -1);
    } while (result == -1 && errno == EINTR);

    if (result == -1)
        return errno;
    else if (result == 0)
        return IO_TIMEOUT;
    else if (type == WAITFD_C && (pfd.revents & (POLLIN | POLLERR)))
        return IO_CLOSED;

    return IO_DONE;
}

/*
** @func GetPeerName
** Gets our connected peer name
*/
int Socket::GetPeerName(Socket::Address & out)
{
    sockaddr_storage peer;
    socklen_t length = sizeof(peer);

    int error = getpeername(this->sockfd, (struct sockaddr *)&peer, &length);

    if (error < 0)
        return -1;

    char name[INET_ADDRSTRLEN];
    char port[6];

    error = getnameinfo((struct sockaddr *)&peer, length, name, INET_ADDRSTRLEN,
                        port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);

    if (error)
        return error;

    out.ip = name;
    out.port = port;

    return 0;
}

bool Socket::IsConnected()
{
    return this->isConnected;
}

/*
** @func GetSockName
** Gets our local socket name
*/
int Socket::GetSockName(Socket::Address & out)
{
    sockaddr_storage peer;
    socklen_t length = sizeof(peer);

    int error = getsockname(this->sockfd, (struct sockaddr *)&peer, &length);

    if (error < 0)
        return -1;

    char name[INET_ADDRSTRLEN];
    char port[6];

    error = getnameinfo((struct sockaddr *)&peer, length, name, INET_ADDRSTRLEN,
                        port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);

    if (error)
        return error;

    out.ip = name;
    out.port = port;

    return 0;
}

void Socket::SetBlocking(bool shouldBlock)
{
    int flags = fcntl(this->sockfd, F_GETFL, 0);

    flags = (shouldBlock) ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;

    fcntl(this->sockfd, F_SETFL, flags);
}

int Socket::_Bind(sockaddr * addr, socklen_t length)
{
    int error = IO_DONE;

    this->SetBlocking(true);

    if (bind(this->sockfd, addr, length) < 0)
        error = errno;

    this->SetBlocking(false);

    return error;
}

int Socket::TryBind(const Socket::Address & host)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;

    addrinfo * resolved;

    const char * hostname = "0.0.0.0";
    const char * port = host.port.c_str();

    if (host.ip != "*")
        hostname = host.ip.c_str();

    int error = getaddrinfo(hostname, port, &hints, &resolved);

    if (error != 0)
    {
        if (resolved)
            freeaddrinfo(resolved);

        return error;
    }

    for (auto item = resolved; item; item = item->ai_next)
    {
        if (this->sockfd == SOCKET_INVALID)
        {
            this->Destroy();

            error = this->Create(item->ai_family, item->ai_socktype, item->ai_protocol);

            if (error)
                continue;

            this->SetBlocking(false);
        }

        error = this->_Bind(item->ai_addr, item->ai_addrlen);

        if (error == 0)
            break;
    }

    freeaddrinfo(resolved);

    return 0;
}

int Socket::_Connect(sockaddr * addr, socklen_t length)
{
    int error;

    if (this->sockfd == SOCKET_INVALID)
        return IO_CLOSED;

    do
    {
        if (connect(this->sockfd, addr, length) == 0)
            return IO_DONE;
    } while ((error = errno) == EINTR);

    if (error != EINPROGRESS && error != EAGAIN)
        return error;

    if (this->timeout.block == 0.0)
        return IO_TIMEOUT;

    if (error == IO_CLOSED)
    {
        if (recv(this->sockfd, (char *)&error, 0, 0) == 0)
            return IO_DONE;
    }

    return error;
}

int Socket::TryConnect(const Socket::Address & peer)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET;

    addrinfo * resolved =  NULL;

    const char * peername = "127.0.0.1";
    const char * port = peer.port.c_str();

    if (peer.ip != "localhost")
        peername = peer.ip.c_str();

    int error = getaddrinfo(peername, port, &hints, &resolved);

    if (error)
    {
        if (resolved)
            freeaddrinfo(resolved);

        return error;
    }

    for (auto item = resolved; item; item = item->ai_next)
    {
        this->timeout.MarkStart();

        if (this->sockfd == SOCKET_INVALID)
        {
            this->Destroy();

            error = this->Create(item->ai_family, item->ai_socktype, item->ai_protocol);

            if (error)
                continue;

            this->SetBlocking(false);
        }

        error = this->_Connect(item->ai_addr, item->ai_addrlen);

        if (error == 0 || this->timeout.block == 0.0)
            break;
    }

    freeaddrinfo(resolved);

    if (error < 0)
        return -1;

    this->isConnected = 1;

    return 0;
}

int Socket::SendData(const char * data, size_t length, size_t * sent)
{
    int error;
    *sent = 0;

    if (this->sockfd == SOCKET_INVALID)
        return IO_CLOSED;

    for (;;)
    {
        long put = (long)send(this->sockfd, data, length, 0);
        LOG("Sending %s / %d length / %ld sent", data, length, put);

        if (put >= 0)
        {
            *sent = put;
            return IO_DONE;
        }

        error = errno;

        if (error == EPIPE)
            return IO_CLOSED;
        else if (error == EPROTOTYPE)
            continue;
        else if (error == EINTR)
            continue;
        else if (error != EAGAIN)
            return error;
        else if ((error = this->Wait(WAITFD_W)) != IO_DONE)
            return error;
    }

    return IO_UNKNOWN;
}

int Socket::Create(int domain, int type, int protocol)
{
    if (domain != AF_UNSPEC)
    {
        this->sockfd = socket(domain, type, protocol);

        if (this->sockfd != SOCKET_INVALID)
            return IO_DONE;

        return errno;
    }

    return 0;
}

const char * Socket::GetError(int error)
{
    switch (error)
    {
        case EADDRINUSE:
            return SOC_ERROR_ADDRESS_IN_USE;
        case EISCONN:
            return SOC_ERROR_IS_CONNECTED;
        case EACCES:
            return SOC_ERROR_PERMISSION_DENIED;
        case ECONNREFUSED:
            return SOC_ERROR_CONNECTION_REFUSED;
        case ECONNABORTED:
            return SOC_ERROR_CONNECTION_ABORTED;
        case ECONNRESET:
            return SOC_ERROR_CONNECTION_RESET;
        case ETIMEDOUT:
            return SOC_ERROR_CONNECTION_TIMEOUT;
        default:
            return strerror(error);
    }
}

const char * Socket::GAIError(int error)
{
    if (!error)
        return NULL;

    switch (error)
    {
        #if defined (__SWITCH__)
            case EAI_AGAIN:
                return SOC_ERROR_TEMP_NAME_FAILURE;
            case EAI_BADFLAGS:
                return SOC_ERROR_BAD_FLAGS;
            case EAI_BADHINTS:
                return SOC_ERROR_BAD_HINTS;
            case EAI_FAIL:
                return SOC_ERROR_FATAL_NAME_FAILURE;
            case EAI_OVERFLOW:
                return SOC_ERROR_BUFFER_OVERFLOW;
            case EAI_PROTOCOL:
                return SOC_ERROR_UNKNOWN_PROTOCOL;
            case EAI_SERVICE:
                return SOC_ERROR_UNSUPPORTED_SERVICE;
            case EAI_SOCKTYPE:
                return SOC_ERROR_UNSUPPORTED_SOCKTYPE;
            case EAI_SYSTEM:
                return strerror(errno);
        #endif
            case EAI_FAMILY:
                return SOC_ERROR_FAMILY_NOT_SUPPORTED;
            case EAI_MEMORY:
                return SOC_ERROR_MEMORY_ALLOCATION;
            case EAI_NONAME:
                return SOC_ERROR_NO_NAME;
            default:
                return gai_strerror(error);
    }
}

void Socket::Destroy()
{
    if (this->sockfd == SOCKET_INVALID)
        return;

    close(this->sockfd);
    this->sockfd = SOCKET_INVALID;
}
