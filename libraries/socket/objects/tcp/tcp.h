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

        std::string GetString();

    private:
        State state = State::STATE_MASTER;
};
