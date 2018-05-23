#include "common/runtime.h"
#include "socket/common.h"

#include "socket/objects/udp/udp.h"

UDP::UDP()
{
	this->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	if (this->sockfd < 0)
		Love::RaiseError("Failed to create UDP socket. %s.", strerror(errno));
	
	memset(&this->address, 0, sizeof(this->address));

	this->address.sin_family = AF_INET;

	int flags, blocking;
	flags = fcntl(this->sockfd, F_GETFL, 0);

	if (flags < 0)
		Love::RaiseError("Failed to get flags for socket.");

	blocking = fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK);
	if (blocking != 0)
		Love::RaiseError("Failed to set non-blocking on socket.");

	int bufferSize = SOCKET_BUFFERSIZE; //8192

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
		{
			this->ip = "127.0.0.1";
			this->address.sin_addr.s_addr = INADDR_LOOPBACK;
		}
		else
		{
			this->ip = destination;
			this->address.sin_addr.s_addr = inet_addr(this->ip.c_str());
		}	
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

int UDP::Send(const char * datagram, size_t length)
{
	int sent = send(this->sockfd, datagram, length, 0);
	
	return sent;
}

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

int UDP::Receive(char * outBuffer)
{
	int length = recv(this->sockfd, outBuffer, SOCKET_BUFFERSIZE, 0);

	if (length <= 0)
		return 0;

	outBuffer[length] = '\0';

	return length;
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

int UDP::SetOption(const string & option, bool enable)
{
	int optionValue = 0;

	if (option == "broadcast")
		optionValue = SO_BROADCAST;
	else if (option == "dontroute")
		optionValue = SO_DONTROUTE;

	if (optionValue != 0)
		setsockopt(this->sockfd, SOL_SOCKET, optionValue, (char *)&enable, sizeof(bool));

	return 0;
}

int UDP::Close()
{
	int success = shutdown(this->sockfd, SHUT_RDWR);

	return success;
}