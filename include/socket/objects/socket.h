class Socket : public Object
{
    public:
        Socket(const std::string & type, int sock);
        void SetTimeout(double duration);
        int Close();

        int GetPeerName(char * ip, int * port);
        int GetSockName(char * ip, int * port);

    private:
        int sock;

    protected:
        //int Bind();
        //int Connect();
        
        const char * ResolveSpecialIP(const std::string & destination);
        int Ready();

        bool IsConnected();
        bool connected;

        struct timeval timeout;

        int sockfd;
};