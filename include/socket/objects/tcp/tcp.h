#pragma once

class TCP : public Socket
{
    public:
        TCP();

        int Accept();
        int Listen();
        void SetSockfd(int sockfd);

        virtual int SetOption(const std::string & option, int value);
};