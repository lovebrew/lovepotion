#pragma once

class TCP : public Socket
{
    public:
        TCP();
        TCP(int sockfd);

        int Accept();
        void Listen();

        virtual int SetOption(const std::string & option, int value);
};