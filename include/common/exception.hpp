#pragma once

#include <cstring>
#include <exception>
#include <memory>
#include <string>

namespace love
{
    class Exception : public std::exception
    {
      public:
        template<typename... FormatArgs>
        Exception(const char* format, FormatArgs&&... args)
        {
            const auto size                = snprintf(nullptr, 0, format, args...);
            std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size + 1);

            snprintf(buffer.get(), size + 1, format, args...);
            this->message = std::string(buffer.get());
        }

        virtual ~Exception() throw();

        inline virtual const char* what() const throw()
        {
            return this->message.c_str();
        }

      private:
        std::string message;
    };
} // namespace love
