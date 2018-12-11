#pragma once

class TCP : public Socket
{
    public:
        TCP();
        TCP(TCPsocket socket);

        int Bind(const std::string & destination, int port);
        int Connect(const string & destination, int port);

        TCPsocket Accept();
        
        void Listen();

    private:
        TCPsocket socket;
};