#pragma once

#include <common/data.hpp>
#include <common/exception.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <vector>

namespace love
{
    class HashFunction
    {
      public:
        static inline uint32_t leftrot(uint32_t x, uint8_t amount)
        {
            return (x << amount) | (x >> (32 - amount));
        }

        static inline uint32_t rightrot(uint32_t x, uint8_t amount)
        {
            return (x >> amount) | (x << (32 - amount));
        }

        static inline uint64_t rightrot(uint64_t x, uint8_t amount)
        {
            return (x >> amount) | (x << (64 - amount));
        }

        enum Function
        {
            FUNCTION_MD5,
            FUNCTION_SHA1,
            FUNCTION_SHA224,
            FUNCTION_SHA256,
            FUNCTION_SHA384,
            FUNCTION_SHA512,
            FUNCTION_MAX_ENUM
        };

        struct Value
        {
            char data[64];
            size_t size;
        };

        static HashFunction* GetHashFunction(Function func);

        virtual ~HashFunction()
        {}

        virtual void Hash(Function func, const char* input, uint64_t length,
                          Value& output) const = 0;

        virtual bool IsSupported(Function func) const = 0;

        // clang-format off
        static constexpr BidirectionalMap functions = {
            "md5",    HashFunction::Function::FUNCTION_MD5,
            "sha1",   HashFunction::Function::FUNCTION_SHA1,
            "sha224", HashFunction::Function::FUNCTION_SHA224,
            "sha256", HashFunction::Function::FUNCTION_SHA256,
            "sha384", HashFunction::Function::FUNCTION_SHA384,
            "sha512", HashFunction::Function::FUNCTION_SHA512
        };
        // clang-format on

      protected:
        HashFunction()
        {}
    };
} // namespace love
