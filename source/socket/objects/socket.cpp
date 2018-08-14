#include "common/runtime.h"

#include "socket/objects/socket.h"

Socket::Socket(int protocol) : Object("Socket")
{
    this->sockfd = socket(AF_INET, protocol, 0);
    this->Create();

    if (protocol == SOCK_DGRAM)
        this->SetType("udp{unconnected}");
    else
    {
        this->SetType("tcp{master}");

        int yes = 1;
        setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    }

    int flags, blocking;
    flags = fcntl(this->sockfd, F_GETFL, 0);

    if (flags < 0)
        Love::RaiseError("Failed to get flags for socket.");

    blocking = fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK);
    if (blocking != 0)
        Love::RaiseError("Failed to set non-blocking on socket.");
}

Socket::Socket(int protocol, int sockfd) : Object("Socket")
{
    this->sockfd = sockfd;
    this->Create();
}

void Socket::Create()
{
    //if (this->sockfd < 0)
    //    Love::RaiseError("Failed to create %s socket. %s.", this->GetType(protocol), strerror(errno));
    
    memset(&this->address, 0, sizeof(this->address));

    this->address.sin_family = AF_INET;

    int bufferSize = SOCKET_BUFFERSIZE; //8192

    //Set socket to use our buffer size for send and receive
    setsockopt(this->sockfd, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(this->sockfd, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));

    this->pollfd.fd = this->sockfd;
    this->pollfd.events = POLLIN | POLLPRI;
    this->pollfd.revents = 0;

    this->timeout = 0;

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

int Socket::Receive(char * outBuffer, const char * pattern, int size)
{
    int length = recv(this->sockfd, outBuffer, size, 0);

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

int Socket::GetPort()
{
    return this->port;
}

string Socket::GetIP()
{
    return this->ip;
}

int Socket::GetSockName(char * ip, int port)
{
    struct sockaddr_in address;
    u32 addressLength = sizeof(address);

    getsockname(this->sockfd, (struct sockaddr *)&address, &addressLength);
    
    strncpy(ip, inet_ntoa(address.sin_addr), 0x40);
    port = ntohs(address.sin_port);
    
    return 1;
}

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

void Socket::SetTimeout(double timeout)
{
    this->timeout = timeout;
}

int Socket::Close()
{
    int success = shutdown(this->sockfd, SHUT_RDWR);
    this->connected = false;

    return success;
}