#include "common/runtime.h"

#include "socket/objects/socket.h"

Socket::Socket(int protocol)
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

    blocking = fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK);
    if (blocking != 0)
        Love::RaiseError("Failed to set non-blocking on socket.");

    int bufferSize = SOCKET_BUFFERSIZE; //8192

    //Set socket to use our buffer size for send and receive
    setsockopt(this->sockfd, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
    setsockopt(this->sockfd, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));

    this->connected = false;
}

int Socket::Connect(const string & ip, int port)
{
    this->SetSocketData(ip, port, false);

    int status = connect(this->sockfd, (const struct sockaddr *)&this->address, sizeof(this->address));

    if (status < 0)
        return -1;

    this->connected = true;

    return 1;
}

int Socket::Bind(const string & ip, int port)
{
    this->SetSocketData(ip, port, true);

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

int Socket::SetOption(const string & option, int value)
{
    //nothing
}

string Socket::GetType(int protocol)
{
    string returnValue = "TCP";
    if (protocol == SOCK_DGRAM)
        returnValue = "UDP";

    return returnValue;
}

void Socket::SetSocketData(const string & destination, int port, bool isServer)
{
    bool allInterfaces = false;
    bool localHost = false;

    if (!isServer)
    {
        localHost = (destination == "localhost");
        
        if (localHost)
        {
            this->ip = "127.0.0.1";
            this->address.sin_addr.s_addr = INADDR_LOOPBACK;
        }
        else
        {
            this->ip = destination;
            this->address.sin_addr.s_addr = inet_addr(this->ip.c_str());
        }    
    }
    else
    {
        allInterfaces = (destination == "*");
        
        if (allInterfaces) 
        {
            this->ip = "0.0.0.0";
            this->address.sin_addr.s_addr = INADDR_ANY;
        }
        else 
        {
            this->ip = destination;
            this->address.sin_addr.s_addr = inet_addr(this->ip.c_str());
        }
    }

    this->port = port;

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