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

#include "common/exception.h"
#include "socket/errors.h"

#include "socket/io.h"
#include "socket/defines.h"

class Socket
{
    public:
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

            double Get()
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

            double GetRetry()
            {
                if (this->block < 0.0 && this->total < 0.0)
                    return -1;
                else if (this->block < 0.0)
                {
                    double t = this->total - this->GetTime() + this->start;
                    return std::max(t, 0.0);
                }
                else if (this->total < 0.0)
                {
                    double t = this->block - this->GetTime() + this->start;
                    return std::max(t, 0.0);
                }
                else
                {
                    double t = this->total - this->GetTime() + this->start;
                    return std::min(this->block, std::max(t, 0.0));
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

        int Wait(int waitType, Timeout * tm);

        void SetBlocking();

        void SetNonBlocking();

        bool SetTimeout(const char * mode, double duration);

        /* Lua Methods */

        const char * TryBind(int type, const Address & host);

        const char * TryConnect(int type, const Address & peer);

        int Send(const char * data, size_t length, size_t * sent, Timeout * tm);

        int Receive(char * buffer, size_t count, size_t * received, Timeout * tm);

        int GetPeerName(Address & address);

        int GetSockName(Address & address);

        Timeout & GetTimeout() {
            return this->timeout;
        }

        /* End */

        void Destroy();

        const char * Create(int type, int domain, int protocol);

        void Close();

        static const char * GetError(int error);

        static const char * GAIError(int error);

        void SetSock(int & sockfd);

        bool IsConnected();

        void SetTimeoutStart() {
            this->timeout.MarkStart();
        }

    protected:
        int sockfd;
        bool isConnected;
        Timeout timeout;

        int family;

        int _Bind(sockaddr * addr, socklen_t length);

        int _Connect(sockaddr * addr, socklen_t length);
};
