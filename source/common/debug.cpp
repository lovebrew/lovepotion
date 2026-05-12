#include "common/debug.hpp"

#include <arpa/inet.h>
#include <csignal>
#include <cstdio>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/_timeval.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/unistd.h>

namespace love
{
    namespace debug
    {
        bool Socket::open(const detail::Connection& connection)
        {
            this->restore();

            this->lsockfd.reset(socket(AF_INET, SOCK_STREAM, 0));

            if (!this->lsockfd)
                return false;

            fcntl(this->lsockfd.get(), F_SETFD, FD_CLOEXEC);

            int yes = 1;
            setsockopt(this->lsockfd.get(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

            sockaddr_in server {};
            server.sin_family      = AF_INET;
            server.sin_port        = htons(connection.port);
            server.sin_addr.s_addr = INADDR_ANY;

            if (bind(this->lsockfd.get(), (sockaddr*)&server, sizeof(server)) < 0)
                return false;

            if (listen(this->lsockfd.get(), MAX_PENDING_CONNECTIONS) < 0)
                return false;

            fd_set set;
            FD_ZERO(&set);
            FD_SET(this->lsockfd.get(), &set);

            timeval timeout {
                .tv_sec  = connection.timeout,
                .tv_usec = 0,
            };

            int ready = select(this->lsockfd.get() + 1, &set, nullptr, nullptr, &timeout);

            // ready == 0: timeout, ready < 0: select() failed
            if (ready <= 0)
                return false;

            detail::UniqueFD client(accept(this->lsockfd.get(), nullptr, nullptr));
            this->lsockfd.reset();

            if (!client)
                return false;

            this->savedfd.reset(dup(STDOUT_FILENO));
            if (!this->savedfd)
                return false;

            std::fflush(stdout);

            if (dup2(client.get(), STDOUT_FILENO) < 0)
            {
                this->restore();
                return false;
            }

            std::signal(SIGPIPE, SIG_IGN);
            this->redirected = true;
            return true;
        }

        void Socket::restore()
        {
            if (this->savedfd)
            {
                std::fflush(stdout);
                dup2(this->savedfd.get(), STDOUT_FILENO);
                this->savedfd.reset();
            }

            this->lsockfd.reset();
            this->redirected = false;
        }

        Socket g_debugSocket;
    } // namespace debug
} // namespace love
