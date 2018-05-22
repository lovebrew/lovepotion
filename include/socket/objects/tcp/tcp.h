class TCP
{
	public:
		TCP();
		TCP(const std::string & ip, int port);

	private:
		int sockfd;

		struct sockaddr_in address;

		std::string ip;
		int port;
}