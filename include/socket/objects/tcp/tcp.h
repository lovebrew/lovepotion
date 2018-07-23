#pragma once

class TCP : public Socket
{
    public:
        TCP();
        TCP(int sockfd);

        int Accept();
        int Listen();
        void SetSockfd(int sockfd);

        int GetPort();

        virtual int SetOption(const std::string & option, int value);
};