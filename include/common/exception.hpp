#pragma once

#include <cstdarg> // vararg
#include <cstdio>  // vsnprintf
#include <cstring> // strncpy
#include <exception>
#include <string>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    class Exception : public std::exception
    {
      public:
        enum class ExceptionType : uint8_t
        {
            Fatal,
            PiepieBroke
        };

        template<typename... FormatArgs>
        Exception(const char* format, FormatArgs&&... args)
        {
            const auto size                = snprintf(nullptr, 0, format, args...);
            std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size + 1);

            snprintf(buffer.get(), size + 1, format, args...);
            message = std::string(buffer.get());
        }

        template<typename... FormatArgs>
        Exception(ExceptionType type, FormatArgs&&... args)
        {
            std::optional<const char*> format = exceptionTypes.Find(type);

            if (!format)
                return Exception("Unknown Error.");

            Exception(*format, std::forward<decltype(args)>(args)...);
        }

        virtual ~Exception() throw();

        inline virtual const char* what() const throw()
        {
            return message.c_str();
        }

      private:
        // clang-format off
        static constexpr BidirectionalMap exceptionTypes = {
            ExceptionType::Fatal,       "test",
            ExceptionType::PiepieBroke, "penis"
        };
        // clang-format on

        std::string message;
    };
} // namespace love
