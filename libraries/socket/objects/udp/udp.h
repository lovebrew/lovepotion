#pragma once

#include "socket/objects/socket/socket.h"

class UDP : public Socket
{
    public:
        UDP(int & success);

        int SetPeerName(const Socket::Address & host);

        int SetSockName(const Socket::Address & peer);

        int Send(const char * data, size_t length, size_t * sent);

        int TryDisconnect();

        std::string GetString();
};
