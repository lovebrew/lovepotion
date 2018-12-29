#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/tcp/tcp.h"

TCP::TCP() : Socket("tcp{master}", SOCK_STREAM) {}

TCP::TCP(int sockfd) : Socket("tcp{client}", SOCK_STREAM)
{
    this->sockfd = sockfd;
}

/*
** Binds the TCP socket to `destination:port`
** '*' should be passed for INADDR_ANY, resolved to NULL
** when using SDL_net
*/

int TCP::Bind(const string & destination, int port)
{
    LOG("Begin Bind");

    struct sockaddr_in address;

    this->sockfd = socket(AF_INET, SOCK_STREAM, 0);

    LOG("Sockfd: %d", this->sockfd);

    if (this->sockfd < 0)
    {
        LOG("Error: %s", strerror(errno));
        return 0;
    }

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    int status = bind(this->sockfd, (struct sockaddr *)&address, sizeof(address));

    LOG("bind: %d", status);

    if (status == SO_ERROR)
        return 0;

    listen(this->sockfd, 32);

    //fcntl(this->sockfd, F_SETFL, O_NONBLOCK);

    this->SetType("tcp{server}");

    LOG("Bind completed.");

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
    
    //fcntl(this->sockfd, F_SETFL, O_NONBLOCK);

    LOG("Connect completed.");

    return 1;
}

int TCP::Accept()
{
    if (!this->Ready())
        return -1;

    struct sockaddr_in address;
    socklen_t length = sizeof(address);

    int newfd = accept(this->sockfd, (struct sockaddr *)&address, &length);

    if (newfd == EINVAL)
        return -1;

    LOG("Accept: %d", newfd);

    return newfd;
}

int TCP::Send(const char * datagram, long length)
{
    LOG("Attempting to Send");
    int sent = 0;

    LOG("Sending index %ld", length);

    sent = send(this->sockfd, datagram, length, 0);

    LOG("Sent %s", datagram);

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
    int ready = this->Ready();

    if (ready == -1)
        return -1;
    else if (ready)
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
    else
        return -2;
}

/*int TCP::SetOption(const string & option, int value)
{
    //int optionValue = 0;

    if (option == "keepalive")
        printf("stubbed");
    else if (option == "linger")
        printf("stubbed");
    else if (option == "reuseaddr")
        printf("stubbed");
    else if (option == "tcp-nodelay")
        printf("stubbed");

    //if (optionValue != 0)
        //setsockopt(this->sockfd, SOL_SOCKET, optionValue, (char *)&enable, sizeof(bool));

    return 0;
}

*/