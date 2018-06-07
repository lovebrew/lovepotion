#pragma once

class Socket : public Object
{
    public:
        Socket() {};
        Socket(int protocol);

        int Connect(const std::string & ip, int port);
        int Bind(const std::string & ip, int port);

        std::string GetSockName();
        std::vector<Variant> GetPeerName();

        int Receive(char * outBuffer);
        int Send(const char * datagram, size_t length);

        int SetOption(const std::string & option, int value);
        
        void SetTimeout(double timeout);

        int Close();

        bool IsConnected();

    private:
        std::string GetType(int protocol);

    protected:
        void SetSocketData(const std::string & destination, int port, bool isServer);

        int sockfd;

        struct sockaddr_in address;
        
        bool connected;
        std::string ip;
        int port;
};