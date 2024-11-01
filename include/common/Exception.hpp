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

#if 1
    #include <cstdio>

    // Macro to log to both stdout and a debug.log file
    #define LOG(format, ...)                                                                  \
        do                                                                                    \
        {                                                                                     \
            std::FILE* outFile      = std::fopen("debug.log", "a");                           \
            static const char* data = "%s %s:%d: " format "\n";                               \
                                                                                              \
            if (outFile)                                                                      \
            {                                                                                 \
                std::fprintf(outFile, data, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
                std::fclose(outFile);                                                         \
            }                                                                                 \
            std::printf(data, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);               \
        } while (0)
#endif
} // namespace love
