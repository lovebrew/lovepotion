#pragma once

#include "socket/objects/socket/socket.h"

class UDP : public Socket
{
    public:
        UDP(int & success);

        int TryDisconnect();

        int ReceiveFrom(std::vector<char> & buffer, Address & address);

        int SendTo(const char * data, size_t length, size_t * sent, Address & address);

        std::string SetOption(const std::string & name, int value);

        std::string GetString();
};
