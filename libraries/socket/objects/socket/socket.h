#pragma once

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>
#include <fcntl.h>

#include <unistd.h>

#define SO_SOCKNAME_ALL "*"
#define SOCKET_INVALID -1

#include "luasocket.h"
#include "common/exception.h"

class Socket
{
    public:
        enum SocketType
        {
            SOCKET_TCP = SOCK_STREAM,
            SOCKET_UDP = SOCK_DGRAM,
            SOCKET_MAX_ENUM
        };

        struct Address
        {
            std::string ip;
            std::string port;
        };

        struct Timeout
        {
            double block;
            double total;
            double start;

            double GetTime()
            {
                timeval value;
                gettimeofday(&value, (timezone *)NULL);

                return value.tv_sec + value.tv_usec / 1.0e6;
            }
        };

        int Ready(int waitType);

        void SetBlocking(bool shouldBlock);

        void SetTimeout(int duration);

        int TryBind(const Address & host);

        int TryConnect(const Address & peer);

        int Create(int type, int domain, int protocol);

        void Destroy();

        void Close();

        int GetPeerName(Address & address);

        int GetSockName(Address & address);

    protected:
        int sockfd;
        bool isConnected;
        Timeout timeout;

        SocketType sockType;
};
