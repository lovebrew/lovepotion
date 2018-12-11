#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/tcp/tcp.h"

TCP::TCP() : Socket("tcp{master}", SOCK_STREAM) {}

TCP::TCP(TCPsocket socket) : Socket("tcp{client}", SOCK_STREAM)
{
    this->socket = socket;
}

/*
** Binds the TCP socket to `destination:port`
** '*' should be passed for INADDR_ANY, resolved to NULL
** when using SDL_net
*/

int TCP::Bind(const string & destination, int port)
{
    IPaddress address;

    const char * resolved = this->ResolveSpecialIP(destination);
    SDLNet_ResolveHost(&address, resolved, port);

    this->socket = SDLNet_TCP_Open(&address);

    if (!this->socket)
        return -1;

    return 0;
}

int TCP::Connect(const string & destination, int port)
{
    IPaddress address;

    const char * resolved = destination.c_str();
    SDLNet_ResolveHost(&address, resolved, port);

    this->socket = SDLNet_TCP_Open(&address);

    if (!this->socket)
        return -1;

    return 0;
}

TCPsocket TCP::Accept()
{
    TCPsocket newSocket = SDLNet_TCP_Accept(this->socket);

    return newSocket;
}

/*TCP::TCP(int sockfd) : Socket(SOCK_STREAM, sockfd) {}

 Waits for a remote connection on the server 
** object and returns a client object representing that connection.


int TCP::Accept()
{
    int event = poll(&this->pollfd, 1, this->timeout);

    if (event <= 0)
        return event;
    else
    {
        int newSocket = accept(this->sockfd,  NULL, NULL);

        if (newSocket < 0)
            return -3;
        else
        {
            int flags, blocking;

            flags = fcntl(newSocket, F_GETFL, 0);
            if (flags < 0)
                Love::RaiseError("Failed to get flags for socket.");

            blocking = fcntl(newSocket, F_SETFL, flags & ~O_NONBLOCK);
            if (blocking != 0)
                Love::RaiseError("Failed to set non-blocking on socket.");
        }

        return newSocket;
    }
}

Specifies the socket is willing to receive connections
** transforming the object into a server object.

void TCP::Listen()
{
    if (listen(this->sockfd, 5))
        printf("listen: %d %s\n", errno, strerror(errno));
}

** Sets options for the TCP object. 
** Options are only needed by low-level or time-critical applications. 
** You should only modify an option if you are sure you need it. 
**
** 'keepalive'
** 'linger'
** 'reuseaddr'
** 'tcp-nodelay'

int TCP::SetOption(const string & option, int value)
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