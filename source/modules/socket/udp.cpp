#include "common/runtime.h"
#include "udp.h"

using love::UDP;

const char * UDP::Init()
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
	this->SetSocketData(destination, port, true);

	int status = bind(this->udpSocket, (const struct sockaddr *)&this->address, sizeof(this->address));

	if (status < 0)
		return -1;

	return 1;
}

int UDP::SetPeerName(const char * destination, int port)
{
	this->SetSocketData(destination, port, false);
	
	int status = connect(this->udpSocket, (const struct sockaddr *)&this->address, sizeof(this->address));

	if (status < 0)
		return -1;

	return 1;
}

int UDP::SetSocketData(const char * destination, int port, bool isServer)
{
	bool allInterfaces = false;
	bool localHost = false;
	
	if (!isServer) 
		localHost = (strncmp(destination, "localhost", 9) == 0) ? true : false;
	else
		allInterfaces = (strncmp(destination, "*", 1) == 0) ? true : false;


	if (!isServer)
	{
		if (localHost)
			this->ip = "127.0.0.1";
		else
			this->ip = strdup(destination);
	}
	else
	{
		if (allInterfaces)
			this->ip = "0.0.0.0";
		else
			this->ip = strdup(destination);
	}

	this->port = port;

	if (isServer)
		if (!allInterfaces)
			this->address.sin_addr.s_addr = inet_addr(this->ip);
		else
			this->address.sin_addr.s_addr = INADDR_ANY;
	else
		this->address.sin_addr.s_addr = inet_addr(this->ip);

	this->address.sin_port = htons(port);
}

char * UDP::Receive()
{
	char * buffer = (char *)calloc(1, 8192);

	int length = recv(this->udpSocket, buffer, 8191, 0);

	if (length <= 0)
	{
		free(buffer);
		return nullptr;
	}

	buffer[length] = '\0';

	return buffer;
}

int UDP::ReceiveFrom(char * outBuffer, char * outAddress, int * outPort)
{
	struct sockaddr_in fromAddress = {0};
	socklen_t addressLength;

	int length = recvfrom(this->udpSocket, outBuffer, 8191, 0, (struct sockaddr *)&fromAddress, &addressLength);

	if (length <= 0)
		return 0;

	outBuffer[length] = '\0';
	
	strncpy(outAddress, inet_ntoa(fromAddress.sin_addr), 0x40);
	*outPort = ntohs(fromAddress.sin_port);

	return length;
}

int UDP::Send(const char * data, size_t len)
{
	int sent = this->SendTo(data, len, this->ip, this->port);

	return sent;
}

int UDP::SendTo(const char * data, size_t len, const char * destination, int port)
{
	printf("SendTo: %s:%d\n", destination, port);

	struct hostent * hostInfo = gethostbyname(destination);

	if (hostInfo == NULL)
		return -2;
	
	struct sockaddr_in addressTo = {0};

	addressTo.sin_addr = *(struct in_addr *)hostInfo->h_addr;
	addressTo.sin_port = htons(port);
	addressTo.sin_family = AF_INET;

	printf("Sending data: %s!\n", data);

	size_t sent = sendto(this->udpSocket, data, len, 0, (struct sockaddr *)&addressTo, sizeof(addressTo));

	printf("Did we do it, Reddit? %dB sent!\n", sent);

	return sent;
}

int UDP::Close()
{
	close(this->udpSocket);
	
	return 0;
}

int UDP::SetOption(const char * option, bool enabled)
{
	int status = 0;
	if (strncmp(option, "broadcast", 9) == 0)
		status = setsockopt(this->udpSocket, SOL_SOCKET, 0x0020, (char *)&enabled, sizeof(bool));
	else if (strncmp(option, "dontroute", 9) == 0)
		status = setsockopt(this->udpSocket, SOL_SOCKET, 0x0010, (char *)&enabled, sizeof(bool));
	
	return status;
}

int UDP::SetTimeOut(int timeoout)
{
	int flags = fcntl(this->udpSocket, F_GETFL, 0);
	if (timeoout != 0)
		flags += ~O_NONBLOCK;

	fcntl(this->udpSocket, F_SETFL, flags);

	return 0;
}
