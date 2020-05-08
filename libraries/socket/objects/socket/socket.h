#pragma once

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>
#include <fcntl.h>

#include <unistd.h>

#define SO_SOCKNAME_ALL "*"
#define SOCKET_INVALID -1

#if defined (_3DS)
    #define SO_KEEPALIVE 0x0008
    #define SO_DONTROUTE 0x0010
#elif defined (__SWITCH__)

#endif

#define WAITFD_R POLLIN
#define WAITFD_W POLLOUT
#define WAITFD_C (POLLIN | POLLOUT)

#define UDP_DATAGRAMSIZE 0x2000

#include "common/exception.h"
#include "socket/errors.h"

#include "socket/io.h"

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

            bool IsZero()
            {
                return this->block == 0.0;
            }

            void MarkStart()
            {
                this->start = this->GetTime();
            }

            double GetRetry()
            {
                double result = -1.0;

                if (this->block < 0.0 && this->total < 0.0)
                    return result;
                else if (this->block < 0.0)
                {
                    result = this->total - this->GetTime() + this->start;
                    return std::max(result, 0.0);
                }
                else if (this->total < 0.0)
                    return this->block;
                else
                {
                    result = this->total - this->GetTime() + this->start;
                    return std::min(this->block, std::max(result, 0.0));
                }
            }

            bool SetTimeout(const char * mode, double time)
            {
                bool result = true;

                switch (*mode)
                {
                    case 'b':
                        this->block = time;
                        break;
                    case 'r':
                    case 't':
                        this->total = time;
                    default:
                        result = false;
                        break;
                }

                return result;
            }
        };

        int Wait(int waitType);

        void SetBlocking(bool shouldBlock);

        bool SetTimeout(const char * mode, double duration);

        /* Lua Methods */

        const char * TryBind(int type, const Address & host);

        const char * TryConnect(int type, const Address & peer);

        int Send(const char * data, size_t length, size_t * sent);

        int Receive(std::vector<char> & buffer, size_t * received);

        int GetPeerName(Address & address);

        int GetSockName(Address & address);

        /* End */

        void Destroy();

        const char * Create(int type, int domain, int protocol);

        void Close();

        static const char * GetError(int error);

        static const char * GAIError(int error);

        bool IsConnected();

    protected:
        int sockfd;
        bool isConnected;
        Timeout timeout;

        SocketType sockType;
        int family;

        int _Bind(sockaddr * addr, socklen_t length);

        int _Connect(sockaddr * addr, socklen_t length);
};
