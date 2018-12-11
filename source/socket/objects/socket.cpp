#include "common/runtime.h"

#include "socket/objects/socket.h"

Socket::Socket(const std::string & type, int sock) : Object(type)
{
    this->connected = false;
    this->timeout = 0.0;

    this->sock = sock;
}

const char * Socket::ResolveSpecialIP(const string & destination)
{
    const char * value = destination.c_str();

    if (destination == "*")
        return NULL;
    else if (destination == "localhost")
        return "127.0.0.1";
    else
        return value;
}

void Socket::GetPeerName(char * ip, int * port)
{

}

void Socket::GetSockName(char * ip, int * port)
{

}

bool Socket::IsConnected()
{
    return this->connected;
}

void Socket::SetTimeout(double timeout)
{
    this->timeout = timeout;
}