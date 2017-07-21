#include "common/runtime.h"
#include "udp.h"

using love::UDP;

char * UDP::Init()
{
	this->udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if (this->udpSocket < 0)
		return "Failed to create socket";

	memset(&this->address, 0, sizeof(this->address));

	this->address.sin_family = AF_INET;
	this->ip = nullptr;

	int flags, blocking;

	flags = fcntl(this->udpSocket, F_GETFL, 0);
	if (flags < 0)
		return "Failed to get flags for socket";

	blocking = fcntl(this->udpSocket, F_SETFL, flags + O_NONBLOCK);
	if (blocking != 0)
		return "Failed to set non-blocking";
	
	int socketSize = 8192;
	setsockopt(this->udpSocket, SOL_SOCKET, SO_RCVBUF, &socketSize, sizeof(socketSize));

	setsockopt(this->udpSocket, SOL_SOCKET, SO_SNDBUF, &socketSize, sizeof(socketSize));

	return nullptr;
}

int UDP::SetSocketName(const char * destination, int port)
{
	this->SetSocketData(destination, port);

	int status = bind(this->udpSocket, (const struct sockaddr *)&this->address, sizeof(this->address));

	if (status < 0)
		return -1;

	return 0;
}

int UDP::SetSocketData(const char * destination, int port)
{
	this->address.sin_addr.s_addr = (unsigned long)destination;
	this->address.sin_port = port;

	this->ip = destination;
	this->port = port;

	printf("Setting Data to %s:%d\n", this->ip, port);
}

int UDP::SetPeerName(const char * destination, int port)
{
	this->SetSocketData(destination, port);

	int status = connect(this->udpSocket, (const struct sockaddr *)&this->address, sizeof(this->address));

	if (status < 0)
		return -1;

	return 0;
}

char * UDP::Receive()
{
	this->buffer = (char *)malloc(8193); //8192 + 1 for null term

	int length = recv(this->udpSocket, buffer, 8193, 0);

	if (length <= 0)
	{
		free(buffer);
		return nullptr;
	}
	else
		*(buffer + length) = 0x0;

	return buffer;
}

int UDP::Send(const char * data, size_t len)
{
	printf("%s:%d\n", this->ip, this->port);

	struct hostent * hostInfo = gethostbyname(this->ip);

	if (hostInfo == nullptr)
		return -2;
	
	struct sockaddr_in addressTo = {0};

	addressTo.sin_addr = *(struct in_addr *)hostInfo->h_addr;
	addressTo.sin_port = htons(this->port);
	addressTo.sin_family = AF_INET;

	size_t sent = sendto(this->udpSocket, data, len, 0, (struct sockaddr *)&addressTo, sizeof(addressTo));

	return sent;
}

int UDP::Close()
{
	close(this->udpSocket);
	
	return 0;
}

int UDP::SetTimeOut(int timeoout)
{
	return 0;
}