#pragma once

class UDP : public Socket
{
    public:
        UDP();

        int SetPeerName(const std::string & ip, int port);
        int SetSockName(const std::string & ip, int port);

        int ReceiveFrom(Datagram & datagram);
        
        virtual int Send(const char * datagram, size_t length);
        int SendTo(const char * datagram, size_t length, const char * destination, int port);

        virtual int SetOption(const std::string & option, int value);

        void SetTimeout(int length);
};