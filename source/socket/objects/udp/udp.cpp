#include "common/runtime.h"

#include "socket/objects/socket.h"
#include "socket/objects/udp/udp.h"

int UDP::SendTo(const char * datagram, size_t len, const char * destination, int port)
{
    FILE * meme = fopen("sdmc:/sendto.txt", "w");

    fprintf(meme, "gethostbyname..\n");
    struct hostent * hostInfo = gethostbyname(destination);
    fprintf(meme, "gethostbyname done!\n");

    fflush(meme);
    if (hostInfo == NULL)
        return -2;
    fprintf(meme, "alright we made it past the NULL check.. good!\n");

    struct sockaddr_in addressTo = {0};

    fprintf(meme, "setting up addressTo sockaddr_in..\n");
    addressTo.sin_addr = *(struct in_addr *)hostInfo->h_addr_list[0];
    addressTo.sin_port = htons(port);
    addressTo.sin_family = AF_INET;
    addressTo.sin_len = sizeof(addressTo);

    fprintf(meme, "done with addressTo stuff, sendingto..\n");
    fflush(meme);
    size_t sent = sendto(this->sockfd, datagram, len, 0, (struct sockaddr *)&addressTo, sizeof(addressTo));
    fprintf(meme, "assuming we even *got* here, let's see: %ldB sent.", sent);
    fflush(meme);

    fclose(meme);

    return sent;
}

int UDP::ReceiveFrom(char * outBuffer, char * outAddress, int * outPort)
{
    struct sockaddr_in fromAddress = {0};
    socklen_t addressLength;

    int length = recvfrom(this->sockfd, outBuffer, SOCKET_BUFFERSIZE, 0, (struct sockaddr *)&fromAddress, &addressLength);

    if (length <= 0)
        return 0;

    outBuffer[length] = '\0';
    
    strncpy(outAddress, inet_ntoa(fromAddress.sin_addr), 0x40);
    *outPort = ntohs(fromAddress.sin_port);

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