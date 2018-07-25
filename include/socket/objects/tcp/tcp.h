#pragma once

class TCP : public Socket
{
    public:
        TCP();
        TCP(int sockfd);

        int Accept();
        void Listen();

        int GetPort();

        virtual int SetOption(const std::string & option, int value);
};