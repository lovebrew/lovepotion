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

    listen(this->sockfd, 5);

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

//Shamelessly stolen from lstlib.c
ptrdiff_t TCP::StringSubPosition(ptrdiff_t pos, size_t len)
{
    /* relative string position: negative means back from end */
    return (pos >= 0) ? pos : (ptrdiff_t)len + pos + 1;
}

int TCP::Send(const char * datagram, size_t length, int i, int j)
{
    LOG("Attempting to Send");

    ptrdiff_t start = this->StringSubPosition(i, length);
    ptrdiff_t end   = this->StringSubPosition(j, length);

    int sent = 0;
    if (start < 1) 
        start = 1;
  
    if (end > (ptrdiff_t)length) 
        end = (ptrdiff_t)length;
    
    const char * tosend = (datagram + start - 1);

    LOG("Sent: %s", tosend);

    if (start <= end)
        sent = send(this->sockfd, datagram + start - 1, end - start + 1, 0);

    LOG("Sent %dB", sent);

    return sent;
}

int TCP::ReceiveLines(char * buffer)
{
    if (!this->Ready())
        return -1;

    char backupbuf[SOCKET_BUFFERSIZE];

    recv(this->sockfd, buffer, sizeof(buffer), MSG_PEEK);

    string temp = buffer;
    int position = temp.find("\r\n");

    //cut off the \r\n
    buffer[position] = '\0';

    //consume the bytes
    return recv(this->sockfd, backupbuf, sizeof(buffer), 0);
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