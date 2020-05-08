#pragma once

#include "socket/objects/socket/socket.h"

class TCP : public Socket
{
    public:
        enum State
        {
            STATE_MASTER,
            STATE_CLIENT,
            STATE_SERVER
        };

        constexpr int DEFAULT_BACKLOG = 32;

        TCP(int & success);

        const char * TryAccept(int * clientfd);

        void SetState(State state);

        int Listen(int backlog);

        void Shutdown(const std::string & how);

        std::string SetOption(const std::string & name, int value);

        std::string GetString();

    private:
        State state = State::STATE_MASTER;
        static std::array<int, 3> shutDownHow;

        int _Accept(int * clientfd, sockaddr * addr, socklen_t length);
};
