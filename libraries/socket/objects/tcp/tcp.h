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

        TCP(int & success, int domain);

        TCP(int * sockfd);

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

        void SetBufferTimeoutStart() {
            this->buffer->timeout->MarkStart();
        }

    private:
        struct Buffer
        {
            char data[BUF_DATASIZE];

            size_t first;
            size_t last;

            size_t sent;
            size_t received;

            double creation;

            Buffer()
            {
                this->received = 0;
                this->sent = 0;

                this->first = 0;
                this->last = 0;
            }

            Timeout * timeout;

            bool IsEmpty()
            {
                return this->first >= this->last;
            }

            void Get(const char ** ctx, size_t * count)
            {
                *count = this->last - this->first;
                *ctx = this->data + this->first;
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

        std::unique_ptr<Buffer> buffer;
};
