#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/udp/udp.h"

int UDP::SendTo(const char * datagram, size_t len, const char * destination, int port)
{
    struct hostent * hostInfo = gethostbyname(destination);

    if (hostInfo == NULL)
        return -2;

    struct sockaddr_in addressTo = {0};

    addressTo.sin_addr = *(struct in_addr *)hostInfo->h_addr_list[0];
    addressTo.sin_port = htons(port);
    addressTo.sin_family = AF_INET;
    addressTo.sin_len = sizeof(addressTo);

    size_t sent = sendto(this->sockfd, datagram, len, 0, (struct sockaddr *)&addressTo, sizeof(addressTo));

    return sent;
}

int UDP::ReceiveFrom(Datagram & datagram)
{
    struct sockaddr_in fromAddress = {0};
    socklen_t addressLength;

    int length = recvfrom(this->sockfd, datagram.buffer, SOCKET_BUFFERSIZE, 0, (struct sockaddr *)&fromAddress, &addressLength);

    if (length <= 0)
        return 0;

    datagram.buffer[length] = '\0';
    
    strncpy(datagram.ip, inet_ntoa(fromAddress.sin_addr), 0x40);
    *datagram.port = ntohs(fromAddress.sin_port);

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