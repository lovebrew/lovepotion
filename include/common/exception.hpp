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
} // namespace love
