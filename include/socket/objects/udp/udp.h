#pragma once

class UDP : public Socket
{
    public:
        UDP();

        int SetPeerName(const std::string & destination, int port);
        int SetSockName(const std::string & destination, int port);

        int ReceiveFrom(char * buffer, char * origin, int * port, size_t bytes);

        int SendTo(const char * datagram, size_t length, const char * destination, int port);

        std::string GetIP();
        int GetPort();

        int Close();

    private:
        UDPsocket socket;
};