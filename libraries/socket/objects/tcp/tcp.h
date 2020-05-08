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

        TCP(int & success);

        int Accept();

        void SetState(State state);

        int Listen(int backlog);

        std::string SetOption(const std::string & name, int value);

        std::string GetString();

    private:
        State state = State::STATE_MASTER;
};
