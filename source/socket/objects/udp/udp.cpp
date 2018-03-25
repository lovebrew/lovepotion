#include "common/runtime.h"
#include "socket/common.h"

#include "socket/objects/udp/udp.h"

UDP::UDP()
{
	this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (this->sockfd < 0)
		throw Exception("Failed to create UDP socket.");

	memset(&this->address, 0, sizeof(this->address));

	this->address.sin_family = AF_INET;

	int flags, blocking;
	flags = fcntl(this->sockfd, F_GETFL, 0);

	if (flags < 0)
		throw Exception("Failed to get flags for socket.");

	blocking = fcntl(this->sockfd, F_SETFL, flags + O_NONBLOCK);
	if (blocking != 0)
		throw Exception("Failed to set non-blocking on socket.");

	int bufferSize = SOCKET_BUFFERSIZE;

	//Set socket to use our buffer size for send and receive
	setsockopt(this->sockfd, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof(bufferSize));
	setsockopt(this->sockfd, SOL_SOCKET, SO_SNDBUF, &bufferSize, sizeof(bufferSize));
}

void UDP::SetSocketData(const string & destination, int port, bool isServer)
{
	bool allInterfaces = false;
	bool localHost = false;

	if (!isServer)
	{
		localHost = (destination == "localhost");
		
		if (localHost)
			this->ip = "127.0.0.1";
		else
			this->ip = destination;

		this->address.sin_addr.s_addr = inet_addr(this->ip.c_str());
	}
	else
	{
		allInterfaces = (destination == "*");
		
		if (allInterfaces) 
		{
			this->ip = "0.0.0.0";
			this->address.sin_addr.s_addr = INADDR_ANY;
		}
		else 
		{
			this->ip = destination;
			this->address.sin_addr.s_addr = inet_addr(this->ip.c_str());
		}
	}

	this->port = port;

	this->address.sin_port = htons(port);
}

int UDP::SetPeerName(const string & ip, int port)
{
	this->SetSocketData(ip, port, false);

	int status = connect(this->sockfd, (const struct sockaddr *)&this->address, sizeof(this->address));

	if (status < 0)
		return -1;

	return 1;
}

int UDP::SetSockName(const string & ip, int port)
{
	this->SetSocketData(ip, port, true);

	int status = bind(this->sockfd, (const struct sockaddr *)&this->address, sizeof(this->address));

	if (status < 0)
		return -1;

	return 1;
}

int UDP::Send(const string & datagram, size_t length)
{
	int sent = this->SendTo(datagram, length, this->ip, this->port);

	return sent;
}

int UDP::SendTo(const string & datagram, size_t length, const string & ip, int port)
{
	struct hostent * hostInfo = gethostbyname(ip.c_str());

	if (hostInfo == NULL)
		return -2;

	struct sockaddr_in addressTo = {0};

	addressTo.sin_addr = *(struct in_addr *)hostInfo->h_addr;
	addressTo.sin_port = htons(port);
	addressTo.sin_family = AF_INET;

	size_t sent = sendto(this->sockfd, datagram.c_str(), length, 0, (struct sockaddr *)&addressTo, sizeof(addressTo));

	return sent;
}

int UDP::ReceiveFrom(char * outBuffer, char * outAddress, int * outPort)
{
	struct sockaddr_in fromAddress = {0};
	socklen_t addressLength;

	int length = recvfrom(this->sockfd, outBuffer, SOCKET_BUFFERSIZE - 1, 0, (struct sockaddr *)&fromAddress, &addressLength);

	if (length <= 0)
		return 0;

	outBuffer[length] = '\0';
	
	strncpy(outAddress, inet_ntoa(fromAddress.sin_addr), 0x40);
	*outPort = ntohs(fromAddress.sin_port);

	return length;
}

int UDP::Close()
{
	int success = shutdown(this->sockfd, SHUT_RDWR);

	return success;
}