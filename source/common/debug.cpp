#include "common/debug.hpp"

namespace love
{
    bool DebugSocket::open(std::string& error)
    {
        return this->open(8000, 3, error);
    }

    bool DebugSocket::open(uint16_t port, int timeout, std::string& error)
    {
        this->closeAll();

        this->lsockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (this->lsockfd < 0)
        {
            error = "Failed to create socket.";
            this->closeAll();
            return false;
        }

        if (auto flags = fcntl(this->lsockfd, F_GETFL); flags != -1)
            fcntl(this->lsockfd, F_SETFL, flags | FD_CLOEXEC);

        const int yes = 1;
        setsockopt(this->lsockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

        sockaddr_in server {};
        server.sin_family      = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port        = htons(port);

        if (bind(this->lsockfd, (struct sockaddr*)&server, sizeof(server)) < 0)
        {
            error = "Failed to bind socket.";
            this->closeAll();
            return false;
        }

        if (listen(this->lsockfd, DebugSocket::MAX_PENDING_CONNECTIONS) < 0)
        {
            error = "Failed to listen on socket.";
            this->closeAll();
            return false;
        }

        fd_set set;
        FD_ZERO(&set);
        FD_SET(this->lsockfd, &set);

        timeval tm {};
        tm.tv_sec  = timeout;
        tm.tv_usec = 0;

        const auto selection = select(this->lsockfd + 1, &set, nullptr, nullptr, &tm);
        if (selection < 0)
        {
            error = "Failed to select on socket.";
            this->closeAll();
            return false;
        }
        else if (selection == 0)
        {
            error = "Timeout while waiting for connection.";
            this->closeAll();
            return false;
        }

        const auto accepted = accept(this->lsockfd, nullptr, nullptr);
        close(this->lsockfd);
        this->lsockfd = -1;

        if (accepted < 0)
        {
            error = "Failed to accept connection.";
            this->closeAll();
            return false;
        }

        this->savedStdoutFd = dup(STDOUT_FILENO);
        if (this->savedStdoutFd < 0)
        {
            error = "Failed to duplicate stdout file descriptor.";
            close(accepted);
            this->closeAll();
            return false;
        }

        std::fflush(stdout);

        if (dup2(accepted, STDOUT_FILENO) < 0)
        {
            error = "Failed to redirect stdout.";
            dup2(this->savedStdoutFd, STDOUT_FILENO);
            close(this->savedStdoutFd);
            this->savedStdoutFd = -1;
            close(accepted);
            this->closeAll();
            return false;
        }

        close(accepted);
        signal(SIGPIPE, SIG_IGN);

        return this->redirected = true;
    }

    void DebugSocket::restore()
    {
        if (this->savedStdoutFd != -1)
        {
            std::fflush(stdout);
            dup2(this->savedStdoutFd, STDOUT_FILENO);
            close(this->savedStdoutFd);
            this->savedStdoutFd = -1;
        }
        this->redirected = false;
    }

    void DebugSocket::closeAll()
    {
        if (this->lsockfd != -1)
        {
            close(this->lsockfd);
            this->lsockfd = -1;
        }
        this->redirected = false;
    }

    void DebugSocket::move(DebugSocket&& other) noexcept
    {
        this->savedStdoutFd = other.savedStdoutFd;
        this->lsockfd       = other.lsockfd;
        this->redirected    = other.redirected;

        other.lsockfd       = -1;
        other.savedStdoutFd = -1;
        other.redirected    = false;
    }
} // namespace love
