#pragma once

class UDP : public Socket
{
    public:
        UDP() : Socket(SOCK_DGRAM) {};

        int SetPeerName(const std::string & ip, int port) { Socket::Connect(ip, port); };
        int SetSockName(const std::string & ip, int port) { Socket::Bind(ip, port); };

        int ReceiveFrom(char * outBuffer, char * outAddress, int * outPort);
        
        int SendTo(const char * datagram, size_t length, const char * destination, int port);

        virtual int SetOption(const std::string & option, int value);

        void SetTimeout(int length);
};