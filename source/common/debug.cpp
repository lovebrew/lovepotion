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

            detail::UniqueFD listenfd(socket(AF_INET, SOCK_STREAM, 0));

            if (!listenfd)
                return false;

            fcntl(listenfd.get(), F_SETFD, FD_CLOEXEC);

            int yes = 1;
            setsockopt(listenfd.get(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

            sockaddr_in server {};
            server.sin_family      = AF_INET;
            server.sin_port        = htons(connection.port);
            server.sin_addr.s_addr = INADDR_ANY;

            if (bind(listenfd.get(), (sockaddr*)&server, sizeof(server)) < 0)
                return false;

            if (listen(listenfd.get(), MAX_PENDING_CONNECTIONS) < 0)
                return false;

            fd_set set;
            FD_ZERO(&set);
            FD_SET(listenfd.get(), &set);

            timeval timeout {
                .tv_sec  = connection.timeout,
                .tv_usec = 0,
            };

            int ready = select(listenfd.get() + 1, &set, nullptr, nullptr, &timeout);

            // ready == 0: timeout, ready < 0: select() failed
            if (ready <= 0)
                return false;

            detail::UniqueFD client(accept(listenfd.get(), nullptr, nullptr));
            listenfd.reset();

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
            return true;
        }

        void Socket::restore()
        {
            if (!this->savedfd)
                return;

            std::fflush(stdout);
            dup2(this->savedfd.get(), STDOUT_FILENO);
            this->savedfd.reset();
        }

        Socket g_debugSocket;
    } // namespace debug
} // namespace love
