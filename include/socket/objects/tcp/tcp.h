#pragma once

class TCP : public Socket
{
    public:
        TCP() : Socket(SOCK_STREAM) {};

        int Accept();
        int Listen();

        virtual int SetOption(const std::string & option, int value);
};