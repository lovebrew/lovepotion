#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <string>

namespace love
{
    class DebugSocket
    {
      public:
        DebugSocket() = default;

        ~DebugSocket()
        {
            this->restore();
            this->closeAll();
        }

        DebugSocket(const DebugSocket&)            = delete;
        DebugSocket& operator=(const DebugSocket&) = delete;

        DebugSocket(DebugSocket&& other) noexcept
        {
            this->move(std::move(other));
        }

        DebugSocket& operator=(DebugSocket&& other) noexcept
        {
            if (this != &other)
            {
                this->restore();
                this->closeAll();
                this->move(std::move(other));
            }
            return *this;
        }

        bool open(std::string& error);

        bool open(uint16_t port, int timeout, std::string& error);

        void restore();

        bool isRedirected() const
        {
            return this->redirected;
        }

      private:
        static constexpr size_t MAX_PENDING_CONNECTIONS = 5;

        void move(DebugSocket&& other) noexcept;

        void closeAll();

        int lsockfd       = -1;
        bool redirected   = false;
        int savedStdoutFd = -1;
    };
} // namespace love
