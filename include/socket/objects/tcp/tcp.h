#pragma once

class TCP : public Socket
{
    public:
        TCP();
        TCP(int sockfd);

        int Bind(const std::string & destination, int port);
        int Connect(const string & destination, int port);

        int Send(const char * datagram, long length);
        
        int ReceiveLinesStripCR(char * out, const char * in, int size);
        
        int ReceiveLines(char ** buffer);
        int ReceiveAll(char * buffer);
        int ReceiveNumber(char * buffer);

        int Accept();
};