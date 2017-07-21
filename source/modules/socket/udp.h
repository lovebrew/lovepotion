#pragma once

namespace love
{
	class UDP
	{
		public:
			char * Init();
			int SetSocketName(const char * destination, int port);
			int Close();
			char * Receive();
			int Send(const char * data, size_t len);
			int SetPeerName(const char * destination, int port);
			int SetSocketData(const char * destination, int port);
			int SetTimeOut(int timeout);

		private:
			int udpSocket;
			struct sockaddr_in address;
			struct hostent * host;
			const char * ip;
			int port;
			char * buffer;
	};
}