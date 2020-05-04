#include "common/runtime.h"
#include "socket/objects/udp/udp.h"

using namespace love;

love::Type UDP::type("UDP", &Socket::type);

UDP::UDP() : Socket()
{
    this->sockfd = SOCKET_INVALID;
    this->timeout = {-1, -1, 0};

    int success = this->Create(SOCK_DGRAM, AF_INET, 0);

    if (success != 0)
        printf("Failed to create socket");

    this->SetBlocking(true);
}

/*
** If address is '*' the system binds to all local interfaces using the constant INADDR_ANY.
** If port is 0, the system chooses an ephemeral port.
** If successful, the method returns 1.
** In case of error, the method returns nil followed by an error message.
** See: http://w3.impa.br/~diego/software/luasocket/udp.html#setsockname
** We'll just return errno and get the string via the wrapper.
*/
int UDP::SetSockName(const Socket::Address & host)
{
    return this->TryBind(host);
}

/*
**  Address can be an IP address or a host name. Port is the port number.
** If address is '*' and the object is connected, the peer association is removed
** and the object becomes an unconnected object again. In that case, the port argument is ignored.
** In case of error the method returns nil followed by an error message.
** In case of success, the method returns 1.
** See: http://w3.impa.br/~diego/software/luasocket/udp.html#setpeername
** We'll just return errno and get the string via the wrapper.
*/
int UDP::SetPeerName(const Socket::Address & peer)
{
    if (this->isConnected && peer.ip == SO_SOCKNAME_ALL)
        return this->TryDisconnect();

    if (!this->isConnected)
        throw love::Exception("expected a connected UDP object.");

    return this->TryConnect(peer);
}

/*
** "Disconnect" a DGRAM Socket
** See: https://github.com/love2d/love/blob/master/src/libraries/luasocket/libluasocket/inet.c#L367
*/
int UDP::TryDisconnect()
{
    sockaddr_in disconnect;

    memset(&disconnect, 0, sizeof(disconnect));

    disconnect.sin_family = AF_UNSPEC;
    disconnect.sin_addr.s_addr = INADDR_ANY;

    int status = connect(this->sockfd, (sockaddr *)&disconnect, sizeof(disconnect));

    return this->isConnected = !(status == 0);
}
