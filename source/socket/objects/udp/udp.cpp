#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/udp/udp.h"

UDP::UDP() : Socket("udp{unconnected}", SOCK_DGRAM) 
{
    this->ip = "127.0.0.1";
    this->port = 25545;
}

/*
** UDP:setsockname
** destination, port
** if port is zero, auto-use a port
** else user a specified port

** SDL2_net notes:
** when given '*' for the IP
** return NULL so it binds to all interfaces
*/

int UDP::SetSockName(const string & destination, int port)
{ 
    this->socket = SDLNet_UDP_Open(port);

    IPaddress address;

    const char * resolved = this->ResolveSpecialIP(destination);
    SDLNet_ResolveHost(&address, resolved, port);

    int channel = SDLNet_UDP_Bind(this->socket, -1, &address);

    this->ip = destination;
    this->port = port;

    if (channel != -1)
        return 0;

    return channel;
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
        SDLNet_UDP_Unbind(this->socket, 0);
        return 0;
    }

    this->socket = SDLNet_UDP_Open(0);

    IPaddress address;

    const char * resolved = this->ResolveSpecialIP(destination);

    SDLNet_ResolveHost(&address, resolved, port);
    
    int channel = SDLNet_UDP_Bind(this->socket, -1, &address);

    this->ip = destination;
    this->port = port;

    if (channel != -1)
        return 0;

    LOG("SDL_Net Error: %s!", SDLNet_GetError());

    return channel;
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
    int sent = 0;

    UDPpacket * packet = SDLNet_AllocPacket(SOCKET_BUFFERSIZE);
    packet->len = length;
    memcpy(packet->data, datagram, length);

    SDLNet_ResolveHost(&packet->address, destination, port); 

    sent = SDLNet_UDP_Send(this->socket, -1, packet);

    SDLNet_FreePacket(packet);

    return sent;
}

/*
** Get data sent from a socket
** Sets origin ip and port for UDP:receivefrom
**/

int UDP::ReceiveFrom(char * buffer, char * origin, int * port, size_t bytes)
{
    int got_packet = 0;
    int recv_size = 0;

    UDPpacket * packet = SDLNet_AllocPacket(bytes);

    got_packet = SDLNet_UDP_Recv(this->socket, packet);

    if (got_packet == 1)
    {
        memcpy(buffer, packet->data, bytes + 1);
        recv_size = strlen(buffer);
        
        if (origin != NULL && port != NULL)
        {
            inet_ntop(AF_INET, &packet->address.host, origin, 0x40);

            *port = SDLNet_Read16(&packet->address.port);
        }
    }

    SDLNet_FreePacket(packet);

    return recv_size;
}

/*
** UDP:close
** Close the socket
** Disconnects the socket as well
*/

int UDP::Close()
{
    SDLNet_UDP_Close(this->socket);
    
    this->socket = NULL;
    this->connected = false;

    return 0;
}

string UDP::GetIP()
{
    return this->ResolveSpecialIP(this->ip);
}

int UDP::GetPort()
{
    return this->port;
}