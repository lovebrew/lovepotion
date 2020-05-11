#include "common/runtime.h"
#include "socket/objects/tcp/tcp.h"

#define CLASS_MASTER "tcp{master}"
#define CLASS_CLIENT "tcp{client}"
#define CLASS_SERVER "tcp{server}"

TCP::TCP(int & success)
{
    const char * error = this->Create(AF_UNSPEC, SOCK_STREAM, 0);

    if (error != NULL)
    {
        success = -1;
        return;
    }

    this->buffer.data.resize(BUF_DATASIZE);
    this->buffer.creation = this->timeout.GetTime();

    success = IO::IO_DONE;
}

TCP::Stats TCP::GetStats()
{
    Stats stats;

    stats.received = this->buffer.received;
    stats.sent = this->buffer.sent;
    stats.creation = this->timeout.GetTime() - this->buffer.creation;

    return stats;
}

void TCP::SetStats(const TCP::Stats & stats)
{
    this->buffer.received = stats.received;
    this->buffer.sent = stats.sent;
    this->buffer.creation = stats.creation;
}

/*-------------------------------------------------------------------------*\
* Sends a block of data (unbuffered)
\*-------------------------------------------------------------------------*/
int TCP::SendRaw(const char * data, size_t count, size_t * sent)
{
    int error = IO::IO_DONE;
    size_t total = 0;

    while (total < count && error == IO::IO_DONE)
    {
        size_t done = 0;
        size_t step = (count - total <= BUF_DATASIZE) ? count - total : BUF_DATASIZE;

        error = this->Send(data + total, step, &done, &this->buffer.timeout);
        total += done;
    }

    *sent = total;
    this->buffer.sent += total;

    return error;
}

/*-------------------------------------------------------------------------*\
* Reads a fixed number of bytes (buffered)
\*-------------------------------------------------------------------------*/
int TCP::ReceiveRaw(size_t wanted, luaL_Buffer * buff)
{
    int error = IO::IO_DONE;
    size_t total = 0;

    while (error == IO::IO_DONE)
    {
        size_t count = 0;
        const char * data;

        if (this->buffer.IsEmpty())
        {
            size_t got;
            error = this->Receive(this->buffer.data, &got, &this->buffer.timeout);

            this->buffer.first = 0;
            this->buffer.last = got;
        }

        this->buffer.Get(&data, &count);
        count = std::min(count, wanted - total);

        luaL_addlstring(buff, data, count);
        this->buffer.Skip(count);

        total += count;

        if (total >= wanted)
            break;
    }

    return error;
}

/*-------------------------------------------------------------------------*\
* Reads everything until the connection is closed (buffered)
\*-------------------------------------------------------------------------*/
int TCP::ReceiveAll(luaL_Buffer * buff)
{
    int error = IO::IO_DONE;
    size_t total = 0;

    while (error == IO::IO_DONE)
    {
        size_t count = 0;
        const char * data;

        if (this->buffer.IsEmpty())
        {
            size_t got;
            error = this->Receive(this->buffer.data, &got, &this->buffer.timeout);

            this->buffer.first = 0;
            this->buffer.last = got;
        }

        this->buffer.Get(&data, &count);
        total += count;

        luaL_addlstring(buff, data, count);
        this->buffer.Skip(count);
    }

    if (error == IO::IO_CLOSED)
    {
        if (total > 0)
            return IO::IO_DONE;
        else
            return IO::IO_CLOSED;
    }
    else
        return error;
}

/*-------------------------------------------------------------------------*\
* Reads a line terminated by a CR LF pair or just by a LF. The CR and LF
* are not returned by the function and are discarded from the buffer
\*-------------------------------------------------------------------------*/
int TCP::ReceiveLine(luaL_Buffer * buff)
{
    int error = IO::IO_DONE;
    this->timeout.MarkStart();

    while (error == IO::IO_DONE)
    {
        const char * data;
        size_t count = 0;
        size_t position = 0;

        if (this->buffer.IsEmpty())
        {
            size_t got;
            error = this->Receive(this->buffer.data, &got, &this->buffer.timeout);

            this->buffer.first = 0;
            this->buffer.last = got;
        }

        this->buffer.Get(&data, &count);

        while (position < count && data[position] != '\n')
        {
            /* ignore \r -- skip it */
            if (data[position] != '\r')
                luaL_addchar(buff, data[position]);

            position++;
        }

        if (position < count)
        {
            this->buffer.Skip(position + 1);
            break;
        }
        else
            this->buffer.Skip(position);
    }

    return error;
}

int TCP::_Accept(int * clientfd, sockaddr * addr, socklen_t length)
{
    if (this->sockfd == SOCKET_INVALID)
        return IO::IO_CLOSED;

    for (;;)
    {
        int error;

        if ((*clientfd = accept(this->sockfd, addr, &length)) != SOCKET_INVALID)
            return IO::IO_DONE;

        error = errno;

        if (error == EINTR)
            continue;
        else if (error != EAGAIN && error != ECONNABORTED)
            return error;
        else if ((error = this->Wait(WAITFD_R, &this->timeout)) != IO::IO_DONE)
            return error;
    }

    return IO::IO_UNKNOWN;
}

const char * TCP::TryAccept(int * clientfd)
{
    sockaddr_storage address;
    socklen_t length = sizeof(sockaddr_in);

    return this->GetError(this->_Accept(clientfd, (sockaddr *)&address, length));
}

int TCP::Listen(int backlog)
{
    int error = IO::IO_DONE;

    if (listen(this->sockfd, backlog))
        error = errno;

    return error;
}

void TCP::Shutdown(const std::string & how)
{
    for (auto item : TCP::shutDownHow)
    {
        if (item.first == how)
        {
            shutdown(this->sockfd, item.second);
            return;
        }
    }
}

void TCP::SetState(State state)
{
    this->state = state;
}

std::string TCP::GetString()
{
    char buffer[56];

    switch (this->state)
    {
        case STATE_MASTER:
            sprintf(buffer, CLASS_MASTER ": %p", this);
            break;
        case STATE_CLIENT:
            sprintf(buffer, CLASS_CLIENT ": %p", this);
            break;
        default:
            sprintf(buffer, CLASS_SERVER ": %p", this);
            break;
    }

    return buffer;
}

std::string TCP::SetOption(const std::string & name, int value)
{
    static const std::array<std::pair<std::string, int>, 4> options =
    {{
        {"keepalive",   SO_KEEPALIVE},
        {"linger",      SO_LINGER   },
        {"reuseaddr",   SO_REUSEADDR},
        {"tcp-nodelay", TCP_NODELAY }
    }};

    for (auto option : options)
    {
        if (option.first == name)
        {
            int level = (option.second == TCP_NODELAY) ? SOL_TCP : SOL_SOCKET;
            if (setsockopt(this->sockfd, level, option.second, (char *)&value, sizeof(value)) < 0)
                return "setsockopt failed";
        }
        else
            return "unsupported option " + name;
    }

    return "";
}

// Static Variable init
std::array<std::pair<std::string, int>, 3> TCP::shutDownHow =
{{
    { "receive", SHUT_RD   },
    { "send",    SHUT_WR   },
    { "both",    SHUT_RDWR }
}};
