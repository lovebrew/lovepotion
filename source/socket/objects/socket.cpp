#include "common/runtime.h"

#include "socket/objects/socket.h"

Socket::Socket(int protocol) : Object("Socket")
{
    this->sockfd = socket(AF_INET, protocol, 0);

    if (this->sockfd < 0)
        Love::RaiseError("Failed to create %s socket. %s.", this->GetType(protocol), strerror(errno));
    
    memset(&this->address, 0, sizeof(this->address));

    this->address.sin_family = AF_INET;

    int flags, blocking;
    flags = fcntl(this->sockfd, F_GETFL, 0);

    if (flags < 0)
        Love::RaiseError("Failed to get flags for socket.");

    blocking = fcntl(this->sockfd, F_SETFL, flags + O_NONBLOCK);
    if (blocking != 0)
        Love::RaiseError("Failed to set non-blocking on socket.");

    int bufferSize = SOCKET_BUFFERSIZE; //8192

    //Set socket to use our buffer size for send and receive
    setsockopt(this->sockfd, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(this->sockfd, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));

    if (protocol == SOCK_DGRAM)
        this->SetType("udp{unconnected}");
    else
        this->SetType("tcp{master}");

    this->connected = false;
}

int Socket::Connect(const string & ip, int port)
{
    this->SetSocketData(ip, port);

    int status = connect(this->sockfd, (const struct sockaddr *)&this->address, sizeof(this->address));

    if (status < 0)
        return -1;

    this->connected = true;

    return 1;
}

int Socket::Bind(const string & ip, int port)
{
    this->SetSocketData(ip, port);

    int status = bind(this->sockfd, (const struct sockaddr *)&this->address, sizeof(this->address));

    if (status < 0)
        return -1;

    return 1;
}

int Socket::Receive(char * outBuffer)
{
    int length = recv(this->sockfd, outBuffer, SOCKET_BUFFERSIZE, 0);

    if (length <= 0)
        return 0;

    outBuffer[length] = '\0';

    return length;
}

int Socket::Send(const char * datagram, size_t length)
{
    int sent = send(this->sockfd, datagram, length, 0);

    return sent;
}

/*int Socket::GetSockName(string & localIP, int port)
{
    u32 ip = gethostid();
    char buffer[0x40];
    
    sprintf(buffer, "%lu.%lu.%lu.%lu", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);

    localIP = buffer;
    port = this->port;

    return 1;
}*/

int Socket::SetOption(const string & option, int value)
{
    //nothing
    return 0;
}

string Socket::GetType(int protocol)
{
    string returnValue = "TCP";
    if (protocol == SOCK_DGRAM)
        returnValue = "UDP";

    return returnValue;
}

void Socket::SetSocketData(const string & destination, int port)
{
    if (destination == "*")
        this->ip = "0.0.0.0";
    else if (destination == "localhost")
        this->ip = "127.0.0.1";
    else
        this->ip = destination;

    this->port = port;

    const char * dest_tmp = this->ip.c_str();

    this->address.sin_addr.s_addr = inet_addr(dest_tmp);
    this->address.sin_port = htons(port);
}

bool Socket::IsConnected()
{
    return this->connected;
}

int Socket::Close()
{
    int success = shutdown(this->sockfd, SHUT_RDWR);
    this->connected = false;

    return success;
}