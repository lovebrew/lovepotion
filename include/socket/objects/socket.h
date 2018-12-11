class Socket : public Object
{
    public:
        Socket(const std::string & type, int sock);
        void SetTimeout(double duration);
        int Close();

    private:
        int sock;

    protected:
        const char * ResolveSpecialIP(const std::string & destination);

        void GetPeerName(char * ip, int * port);
        void GetSockName(char * ip, int * port);

        bool IsConnected();
        bool connected;

        double timeout;

        std::string ip;
        int port;
};