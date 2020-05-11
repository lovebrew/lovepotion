#pragma once

#include "socket/objects/socket/socket.h"

class TCP : public Socket
{
    public:
        enum State
        {
            STATE_MASTER,
            STATE_CLIENT,
            STATE_SERVER
        };

        struct Stats
        {
            size_t received;
            size_t sent;
            double creation;
        };

        static constexpr int DEFAULT_BACKLOG = 32;

        TCP(int & success);

        int SendRaw(const char * data, size_t count, size_t * sent);

        int ReceiveLine(luaL_Buffer * buff);

        int ReceiveAll(luaL_Buffer * buff);

        int ReceiveRaw(size_t wanted, luaL_Buffer * buffer);

        Stats GetStats();

        void SetStats(const Stats & stats);

        const char * TryAccept(int * clientfd);

        void SetState(State state);

        int Listen(int backlog);

        void Shutdown(const std::string & how);

        std::string SetOption(const std::string & name, int value);

        std::string GetString();

    private:
        struct Buffer
        {
            std::vector<char> data;
            size_t received = 0;
            size_t sent = 0;

            size_t first = 0;
            size_t last = 0;

            double creation;
            Timeout timeout;

            bool IsEmpty()
            {
                return this->first >= this->last;
            }

            void Get(const char ** ctx, size_t * count)
            {
                *count = this->last - this->first;
                *ctx = this->data.data() + this->first;
            }

            void Skip(size_t count)
            {
                this->received += count;
                this->first += count;

                if (this->IsEmpty())
                    this->first = this->last = 0;
            }
        };

        State state = State::STATE_MASTER;
        static std::array<std::pair<std::string, int>, 3> shutDownHow;

        int _Accept(int * clientfd, sockaddr * addr, socklen_t length);

        Buffer buffer;
};
