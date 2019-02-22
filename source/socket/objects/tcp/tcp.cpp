#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/tcp/tcp.h"

TCP::TCP() : Socket("tcp{master}", SOCK_STREAM) {}

TCP::TCP(int sockfd) : Socket("tcp{client}", SOCK_STREAM)
{
    this->sockfd = sockfd;
    this->SetBlocking(false);
}

/*
** Binds the TCP socket to `destination:port`
** '*' should be passed for INADDR_ANY, resolved to NULL
** when using SDL_net
*/

int TCP::Bind(const string & destination, int port)
{
    struct sockaddr_in address;

    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->sockfd < 0)
        return 0;

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    int status = bind(this->sockfd, (struct sockaddr *)&address, sizeof(address));

    if (status == SO_ERROR)
        return 0;

    listen(this->sockfd, 32);

    this->SetType("tcp{server}");
    
    this->SetBlocking(false);

    return 1;
}

int TCP::Connect(const string & destination, int port)
{
    struct hostent * host;
    struct sockaddr_in address;

    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->sockfd < 0)
        return 0;

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    const char * name = destination.c_str();
    host = gethostbyname(name);

    memcpy((void *)&address.sin_addr, host->h_addr_list[0], host->h_length);

    int status = connect(this->sockfd, (struct sockaddr *)&address, sizeof(address));

    if (status == SO_ERROR)
        return 0;
    
    this->SetBlocking(false);

    return 1;
}

int TCP::Accept()
{
    struct sockaddr_in address;
    socklen_t length = sizeof(address);

    int newfd = accept(this->sockfd, (struct sockaddr *)&address, &length);

    if (newfd == EINVAL)
        return -1;

    return newfd;
}

int TCP::Send(const char * datagram, long length)
{
    int sent = 0;
    
    sent = send(this->sockfd, datagram, length, 0);

    return sent;
}

int TCP::ReceiveLinesStripCR(char * outbuf, const char * inbuf, int inbuf_size)
{
    char * pos;
    int used_size = 0;
    int amount_of_cr = 0;
 
    while((pos = strchr(inbuf + used_size, '\r')) != NULL)
    {
        ptrdiff_t length_until_cr = pos - (inbuf + used_size);
        if(length_until_cr)
        {
            memcpy(outbuf, (inbuf + used_size), length_until_cr);
            outbuf += length_until_cr;
        }
        amount_of_cr++;
        used_size += length_until_cr + 1;  // jump over the \r we just saw
    }
 
    // at worst, inbuf + used_size points to the terminating NUL, so this will end the outbuf string correctly
    // and if pos was NULL from the start (no CR in inbuf), it can't overflow outbuf anyway because it's just the right size
    strcpy(outbuf, inbuf + used_size);
 
    return used_size - amount_of_cr;
}

int TCP::ReceiveLines(char ** outbuf)
{
    int ready = this->Ready(POLLIN);

    if (ready != 0)
        return ready;
    else if (ready == EINTR)
        return ready;
    else if (errno != EAGAIN)
        return errno;
    else
    {
        //0x2000
        char recvbuf[SOCKET_BUFFERSIZE];
        char * recvbuf_ptr = recvbuf;

        bool done = false;
        char * pos;
        int out_size = 0;
        int read_size_before_newline = 0;
    
        do
        {
            recv(this->sockfd, recvbuf, sizeof(recvbuf), MSG_PEEK);
            
            if((pos = strchr(recvbuf, '\n')) != NULL)
            {
                ptrdiff_t length_until_newline = pos - recvbuf_ptr;  // ensure recvbuf gets treated as a pointer
                recvbuf[length_until_newline] = '\0';  // Replace the LF with a NUL to make stripping the CRs easier
                // + 1  for the terminating NUL
                *outbuf = (char *)malloc(length_until_newline + 1);
                // string size, without CRs
                out_size = this->ReceiveLinesStripCR(*outbuf, recvbuf, length_until_newline);
                // + 1  for eliminating the LF when you consume later
                read_size_before_newline = length_until_newline + 1;
                done = true;
            }

        } while (!done);
    
        //consume the bytes
        int to_consume = read_size_before_newline;
        do 
        {
            int to_receive = to_consume > SOCKET_BUFFERSIZE ? SOCKET_BUFFERSIZE : to_consume;
            recv(this->sockfd, recvbuf, to_receive, 0);
            to_consume -= to_receive;
        } while (to_consume);
    
        return out_size;
    }
}

int TCP::ReceiveNumber(char ** outbuf, size_t length)
{
    int ready = this->Ready(POLLIN);
    int recvd = 0;
    
    if (ready != 0)
        return ready;
    else if (ready == EINTR)
        return ready;
    else if (errno != EAGAIN)
        return errno;
    else
    {
        *outbuf = (char *)malloc(length);
        recvd = recv(this->sockfd, *outbuf, length, 0);

        return recvd;
    }
}

int TCP::SetOption(const string & option, int enable)
{
    int value = 0;
    int success = 0;

    if (option == "keepalive")
        value = SO_KEEPALIVE;
    else if (option == "linger")
        value = SO_LINGER;
    else if (option == "reuseaddr")
        value = SO_REUSEADDR;
    else if (option == "tcp-nodelay")
        value = TCP_NODELAY;

    if (value != 0)
        success = setsockopt(this->sockfd, SOL_SOCKET, value, (char *)&enable, sizeof(enable));

    return success;
}