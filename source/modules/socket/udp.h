#pragma once

namespace love
{
	class UDP
	{
		public:
			const char * Init();
			int SetSocketName(const char * destination, int port);
			int Close();
			char * Receive();
			int ReceiveFrom(char * outBuffer, char * outAddress, int * outPort);
			int Send(const char * data, size_t len);
			int SendTo(const char * data, size_t len, const char * destination, int port);
			int SetOption(const char * option, bool enable);
			int SetPeerName(const char * destination, int port);
			int SetSocketData(const char * destination, int port, bool isServer);
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
