#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/udp/udp.h"

UDP::UDP() : Socket("udp{unconnected}", SOCK_DGRAM) {}

/*
** UDP:setsockname
** destination, port
** if port is zero, auto-use a port
** else user a specified port
*/

int UDP::SetSockName(const string & destination, int port)
{
    struct sockaddr_in address;

    this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (this->sockfd < 0)
        return -1;

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    int status = bind(this->sockfd, (struct sockaddr *)&address, sizeof(address));
    
    if (status == SO_ERROR)
        return -1;

    return 1;
}

/*
** UDP:setsockname
** destination, port
** if port is zero, auto-use a port
** else user a specified port
**
** if connected already
** and '*' is used for 'destination'
** disconnect the socket
*/

int UDP::SetPeerName(const string & destination, int port)
{
    if (this->IsConnected() && destination == "*")
    {
        //SDLNet_UDP_Unbind(this->socket, 0);
        return 0;
    }

    struct hostent * host;
    struct sockaddr_in address;

    this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (this->sockfd < 0)
        return -1;

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    const char * name = destination.c_str();
    host = gethostbyname(name);

    memcpy((void *)&address.sin_addr, host->h_addr_list[0], host->h_length);

    int status = connect(this->sockfd, (struct sockaddr *)&address, sizeof(address));
    
    if (status == SO_ERROR)
        return -1;

    return 1;
}

/*
** Sent a string to 'destination' on its 'port'
**
** SDL2_net notes:
** Allocate the packet to 0x2000 (LuaSocket size)
** Set the packet data length
** Set the packet data string
** Resolve the destination/port data for the packet
** Send the packet
** Free the packet (don't want a memeleak)
*/

int UDP::SendTo(const char * datagram, size_t length, const char * destination, int port)
{
    struct sockaddr_in address;
    int socketSize = sizeof(address);

    int sent = 0;

    if (destination == NULL)
        return send(this->sockfd, datagram, length, 0);

    struct hostent * host = gethostbyname(destination);
    memcpy((void *)&address.sin_addr, host->h_addr_list[0], host->h_length);

    address.sin_port = htons(port);
    address.sin_family = AF_INET;

    sent = sendto(this->sockfd, datagram, length, 0, (struct sockaddr *)&address, socketSize);
    
    return sent;
}

/*
** Get data sent from a socket
** Sets origin ip and port for UDP:receivefrom
**/

int UDP::ReceiveFrom(char * buffer, char * origin, int * port, size_t bytes)
{
    if (!this->Ready())
        return 0;

    struct sockaddr_in address;
    socklen_t socketSize = sizeof(address);

    int recvd = 0;

    if (origin == NULL)
    {
        recvd = recv(this->sockfd, buffer, bytes, 0);
        
        buffer[recvd] = '\0';

        return recvd;
    }
    else
    {
        recvd = recvfrom(this->sockfd, buffer, bytes, 0, (struct sockaddr *)&address, &socketSize);

        buffer[recvd] = '\0';

        inet_ntop(AF_INET, &address.sin_addr.s_addr, origin, 0x40);

        *port = ntohs(address.sin_port);
    }

    return recvd;
}