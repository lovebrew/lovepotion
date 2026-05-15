#pragma once

#include <cstdint>
#include <unistd.h>

#include <string>
#include <utility>

namespace love
{
    namespace debug
    {
        namespace detail
        {
            class UniqueFD
            {
              public:
                UniqueFD() = default;

                explicit UniqueFD(int fd) : fd(fd)
                {}

                ~UniqueFD()
                {
                    this->reset();
                }

                UniqueFD(const UniqueFD&)            = delete;
                UniqueFD& operator=(const UniqueFD&) = delete;

                UniqueFD(UniqueFD&& other) noexcept : fd(std::exchange(other.fd, -1))
                {}

                UniqueFD& operator=(UniqueFD&& other) noexcept
                {
                    if (this != &other)
                        this->reset(std::exchange(other.fd, -1));

                    return *this;
                }

                void reset(int value = -1) noexcept
                {
                    if (value != -1)
                        close(fd);

                    this->fd = value;
                }

                int release() noexcept
                {
                    return std::exchange(this->fd, -1);
                }

                int get() const noexcept
                {
                    return this->fd;
                }

                explicit operator bool() const noexcept
                {
                    return this->fd != -1;
                }

              private:
                int fd = -1;
            };

            struct Connection
            {
                uint16_t port  = DEFAULT_PORT;
                size_t timeout = DEFAULT_TIMEOUT_SECONDS;

              private:
                static constexpr uint16_t DEFAULT_PORT          = 8000;
                static constexpr size_t DEFAULT_TIMEOUT_SECONDS = 3;
            };
        } // namespace detail

        class Socket
        {
          public:
            Socket() = default;

            ~Socket()
            {
                this->restore();
            }

            Socket(const Socket&)            = delete;
            Socket& operator=(const Socket&) = delete;

            Socket(Socket&&) noexcept            = delete;
            Socket& operator=(Socket&&) noexcept = delete;

            bool open(const detail::Connection& connection);

            void restore();

          private:
            static constexpr uint8_t MAX_PENDING_CONNECTIONS = 5;
            detail::UniqueFD savedfd;
        };

        extern Socket g_debugSocket;
    } // namespace debug
#if defined(__DEBUG__)
    #include <cstdio>
    #define LOG(format, ...)                                  \
        do                                                    \
        {                                                     \
            std::printf("[C++] " format "\n", ##__VA_ARGS__); \
        } while (0)
#else
    #define LOG(format, ...) \
        do                   \
        {                    \
        } while (0)
#endif
} // namespace love
