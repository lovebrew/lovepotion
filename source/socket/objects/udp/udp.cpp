#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/udp/udp.h"

UDP::UDP() : Socket(SOCK_DGRAM) {}

int UDP::SetSockName(const string & ip, int port)
{
    int success = Socket::Bind(ip, port);

    return success;
}

int UDP::SetPeerName(const string & ip, int port)
{
    int success = Socket::Connect(ip, port);

    return success;
}

int UDP::SendTo(const char * datagram, size_t length, const char * destination, int port)
{
    struct hostent * hostInfo = gethostbyname(destination);

    if (hostInfo == NULL)
        return -2;

    struct sockaddr_in addressTo = {0};

    addressTo.sin_addr = *(struct in_addr *)hostInfo->h_addr;
    addressTo.sin_port = htons(port);
    addressTo.sin_family = AF_INET;

    socklen_t addressLength = sizeof(addressTo);

    size_t sent = sendto(this->sockfd, datagram, length, 0, (struct sockaddr *)&addressTo, addressLength);
    
    return sent;
}

int UDP::ReceiveFrom(Datagram & datagram, size_t bytes)
{
    struct sockaddr_in fromAddress = {0};
    socklen_t addressLength = sizeof(fromAddress);

    int length = recvfrom(this->sockfd, datagram.buffer, bytes, 0, (struct sockaddr *)&fromAddress, &addressLength);

    if (length <= 0)
        return 0;

    datagram.buffer[length] = '\0';
    
    strncpy(datagram.ip, inet_ntoa(fromAddress.sin_addr), 0x40);
    datagram.port = ntohs(fromAddress.sin_port);

    return length;
}

int UDP::SetOption(const string & option, int enable)
{
    int optionValue = 0;

    if (option == "broadcast")
        optionValue = SO_BROADCAST;
    else if (option == "dontroute")
        optionValue = SO_DONTROUTE;

    if (optionValue != 0)
        setsockopt(this->sockfd, SOL_SOCKET, optionValue, (char *)&enable, sizeof(enable));

    return 0;
}