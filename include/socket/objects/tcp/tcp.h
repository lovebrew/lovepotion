#pragma once

class TCP : public Socket
{
    public:
        TCP();
        TCP(int sockfd);

        int Bind(const std::string & destination, int port);
        int Connect(const string & destination, int port);

        int Send(const char * datagram, size_t length, int i, int j);
        
        int ReceiveLines(char * buffer);
        int ReceiveAll(char * buffer);
        int ReceiveNumber(char * buffer);

        int Accept();

    private:
        ptrdiff_t StringSubPosition(ptrdiff_t start, size_t length);
};