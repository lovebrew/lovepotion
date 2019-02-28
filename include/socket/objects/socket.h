class Socket : public Object
{
    public:
        Socket(const std::string & type, int sock);
        ~Socket();
        
        void SetTimeout(int duration);
        int Close();

        int GetPeerName(char * ip, int * port);
        int GetSockName(char * ip, int * port);

    private:
        int sock;

    protected:
        //int Bind();
        //int Connect();
        
        const char * ResolveSpecialIP(const std::string & destination);
        int Ready(int wait_type);

        bool IsConnected();
        void SetBlocking(bool block);

        bool connected;

        int timeout;

        int sockfd;
};
