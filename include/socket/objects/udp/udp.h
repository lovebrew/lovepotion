#pragma once

class UDP
{
	public:
		UDP();

		void GetSockName();
		int SetSockName(const std::string & ip, int port);

		void Receive();
		int ReceiveFrom(char * outBuffer, char * outAddress, int * outPort);
		
		int Send(const std::string & datagram, size_t length);
		int SendTo(const std::string & datagram, size_t length, const std::string & ip, int port);
		
		int SetPeerName(const std::string & ip, int port);
		void GetPeerName();

		int SetOption(const std::string & option, bool enable);

		void SetTimeout(int length);

		int Close();

	private:
		void SetSocketData(const std::string & destination, int port, bool isServer);

		int sockfd;

		struct sockaddr_in address;
		struct hosten * host;
		
		std::string ip;
		int port;
		
		char * buffer;
};