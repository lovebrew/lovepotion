#include "common/runtime.h"
#include "socket/objects/socket/socket.h"

int Socket::Wait(int type, Timeout * tm)
{
    pollfd pfd;

    pfd.fd = this->sockfd;
    pfd.events = type;
    pfd.revents = 0;

    int result;

    if (tm->IsZero())
        return IO::IO_TIMEOUT;

    do
    {
        int t = (int)(tm->GetRetry() * 1e3);
        result = poll(&pfd, 1, t >= 0 ? t : -1);
    } while (result == -1 && errno == EINTR);

    if (result == -1)
        return errno;

    if (result == 0)
        return IO::IO_TIMEOUT;

    if (type == WAITFD_C && (pfd.revents & (POLLIN | POLLERR)))
        return IO::IO_CLOSED;

    return IO::IO_DONE;
}

void Socket::SetSock(int & sockfd)
{
    this->sockfd = sockfd;
}

bool Socket::SetTimeout(const char * mode, double duration)
{
    return this->timeout.SetTimeout(mode, duration);
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

void Socket::SetBlocking()
{
    int flags = fcntl(this->sockfd, F_GETFL, 0);

    flags &= ~O_NONBLOCK;

    fcntl(this->sockfd, F_SETFL, flags);
}

void Socket::SetNonBlocking()
{
    int flags = fcntl(this->sockfd, F_GETFL, 0);

    flags |= O_NONBLOCK;

    fcntl(this->sockfd, F_SETFL, flags);
}

int Socket::_Bind(sockaddr * addr, socklen_t length)
{
    int error = IO::IO_DONE;

    this->SetBlocking();

    if (bind(this->sockfd, addr, length) < 0)
        error = errno;

    this->SetNonBlocking();

    return error;
}

const char * Socket::TryBind(int type, const Socket::Address & host)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_socktype = type;
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;

    addrinfo * resolved = NULL;
    const char * error = NULL;

    const char * hostname = "0.0.0.0";
    const char * port = host.port.c_str();

    if (host.ip != "*")
        hostname = host.ip.c_str();

    error = this->GAIError(getaddrinfo(hostname, port, &hints, &resolved));

    if (error)
    {
        if (resolved)
            freeaddrinfo(resolved);

        return error;
    }

    for (auto item = resolved; item; item = item->ai_next)
    {
        if (this->family != item->ai_family || this->sockfd == SOCKET_INVALID)
        {
            this->Destroy();

            error = this->Create(item->ai_family, item->ai_socktype, item->ai_protocol);

            if (error)
                continue;

            this->family = item->ai_family;
        }

        error = this->GetError(this->_Bind(item->ai_addr, item->ai_addrlen));

        if (error == NULL)
        {
            this->SetNonBlocking();
            break;
        }
    }

    freeaddrinfo(resolved);

    return error;
}

int Socket::_Connect(sockaddr * addr, socklen_t length)
{
    int error;

    if (this->sockfd == SOCKET_INVALID)
        return IO::IO_CLOSED;

    do
    {
        if (connect(this->sockfd, addr, length) == 0)
            return IO::IO_DONE;
    } while ((error = errno) == EINTR);

    if (error != EINPROGRESS && error != EAGAIN)
        return error;

    if (this->timeout.block == 0.0)
        return IO::IO_TIMEOUT;

    error = this->Wait(WAITFD_C, &this->timeout);

    if (error == IO::IO_CLOSED)
    {
        if (recv(this->sockfd, (char *)&error, 0, 0) == 0)
            return IO::IO_DONE;
        else
            return errno;
    }
    else
        return error;
}

const char * Socket::TryConnect(int type, const Socket::Address & peer)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_socktype = type;
    hints.ai_family = AF_INET;

    addrinfo * resolved =  NULL;
    const char * error = NULL;

    const char * peername = "127.0.0.1";

    if (peer.ip != "localhost")
        peername = peer.ip.c_str();

    const char * port = peer.port.c_str();

    error = this->GAIError(getaddrinfo(peername, port, &hints, &resolved));

    if (error)
    {
        if (resolved)
            freeaddrinfo(resolved);

        return error;
    }

    for (auto item = resolved; item; item = item->ai_next)
    {
        this->timeout.MarkStart();

        if (this->family != item->ai_family || this->sockfd == SOCKET_INVALID)
        {
            this->Destroy();

            error = this->Create(item->ai_family, item->ai_socktype, item->ai_protocol);

            if (error)
                continue;

            this->family = item->ai_family;
            this->SetNonBlocking();
        }

        error = this->GetError(this->_Connect(item->ai_addr, item->ai_addrlen));

        if (error == NULL || this->timeout.IsZero())
            break;
    }

    freeaddrinfo(resolved);

    this->isConnected = 1;

    return error;
}

int Socket::Receive(char * buffer, size_t count, size_t * received, Timeout * tm)
{
    int error;
    *received = 0;

    if (this->sockfd == SOCKET_INVALID)
        return IO::IO_CLOSED;

    for (;;)
    {
        long taken = (long)recv(this->sockfd, buffer, count, 0);

        if (taken > 0)
        {
            *received = taken;
            return IO::IO_DONE;
        }

        error = errno;

        if (taken == 0)
            return IO::IO_CLOSED;

        if (error == EINTR)
            continue;

        if (error != EAGAIN)
            return error;

        if ((error = this->Wait(WAITFD_R, tm)) != IO::IO_DONE)
            return error;
    }

    return IO::IO_UNKNOWN;
}

int Socket::Send(const char * data, size_t length, size_t * sent, Timeout * tm)
{
    int error;
    *sent = 0;

    if (this->sockfd == SOCKET_INVALID)
        return IO::IO_CLOSED;

    for (;;)
    {
        long put = (long)send(this->sockfd, data, length, 0);

        if (put >= 0)
        {
            *sent = put;
            return IO::IO_DONE;
        }

        error = errno;

        if (error == EPIPE)
            return IO::IO_CLOSED;

        if (error == EPROTOTYPE)
            continue;

        if (error == EINTR)
            continue;

        if (error != EAGAIN)
            return error;

        if ((error = this->Wait(WAITFD_W, tm)) != IO::IO_DONE)
            return error;
    }

    return IO::IO_UNKNOWN;
}

const char * Socket::Create(int domain, int type, int protocol)
{
    this->sockfd = SOCKET_INVALID;

    this->timeout =
    {
        .block = -1.0,
        .total = -1.0,
        .start = 0.0
    };

    this->family = domain;

    if (domain != AF_UNSPEC)
    {
        this->sockfd = socket(domain, type, protocol);

        this->SetNonBlocking();

        if (this->sockfd != SOCKET_INVALID)
            return NULL;

        return this->GetError(errno);
    }

    return NULL;
}

const char * Socket::GetError(int error)
{
    if (error <= 0)
        return IO::GetError(error);

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
    if (this->sockfd != SOCKET_INVALID)
    {
        close(this->sockfd);
        this->sockfd = SOCKET_INVALID;
    }
}
