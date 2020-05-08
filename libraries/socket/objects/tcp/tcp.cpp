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

    this->SetBlocking(false);
    success = IO::IO_DONE;
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
    std::array<std::pair<std::string, int>, 4> options =
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
            if (setsockopt(this->sockfd, SOL_TCP, option.second, (char *)value, sizeof(value)) < 0)
                return "setsockopt failed";
        }
        else
            return "unsupported option " + name;
    }

    return "";
}
