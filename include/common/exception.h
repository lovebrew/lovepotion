#pragma once

#include <cstdarg> // vararg
#include <cstdio>  // vsnprintf
#include <cstring> // strncpy
#include <exception>
#include <string>

namespace love
{
    class Exception : public std::exception
    {
      public:
        Exception(const char* format, ...);
        virtual ~Exception() throw();

        inline virtual const char* what() const throw()
        {
            return message.c_str();
        }

      private:
        std::string message;
    };
} // namespace love
