#pragma once

class UDP
{
	public:
		UDP();

		void GetSockName();
		int SetSockName(const std::string & ip, int port);

		int Receive(char * outBuffer);
		int ReceiveFrom(char * outBuffer, char * outAddress, int * outPort);
		
		int Send(const char * datagram, size_t length);
		int SendTo(const char * datagram, size_t length, const char * destination, int port);
		
		int SetPeerName(const std::string & ip, int port);
		void GetPeerName();

		int SetOption(const std::string & option, bool enable);

		void SetTimeout(int length);

		int Close();

	private:
		void SetSocketData(const std::string & destination, int port, bool isServer);

		int sockfd;

		struct sockaddr_in address;
		
		std::string ip;
		int port;
};