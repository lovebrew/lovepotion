#pragma once

#include "common/error.hpp"

#include <exception>
#include <format>

namespace love
{
    class Exception : public std::exception
    {
      public:
        template<typename... Args>
        Exception(const char* format, Args&&... args)
        {
            this->message = std::vformat(format, std::make_format_args(args...));
        }

        virtual ~Exception() throw()
        {}

        inline virtual const char* what() const throw()
        {
            return this->message.c_str();
        }

      private:
        std::string message;
    };

#if __DEBUG__
    #include <cstdio>
    #define __FILENAME__ (__FILE__ + (sizeof(__FILE__) - 1) - sizeof(basename(__FILE__)))

    // Macro to log to both stdout and a debug.log file
    #define LOG(format, ...)                                                        \
        do                                                                          \
        {                                                                           \
            static const char* data = "[C++] %s %s:%d: " format "\n";               \
            std::printf(data, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        } while (0)
#endif
} // namespace love
