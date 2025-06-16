#pragma once

#include "common/Data.hpp"
#include "common/int.hpp"

#include "common/Map.hpp"

namespace love
{
    inline uint32_t leftrot(uint32_t x, uint8_t amount)
    {
        return (x << amount) | (x >> (32 - amount));
    }

    inline uint32_t rightrot(uint32_t x, uint8_t amount)
    {
        return (x >> amount) | (x << (32 - amount));
    }

    inline uint64_t rightrot(uint64_t x, uint8_t amount)
    {
        return (x >> amount) | (x << (64 - amount));
    }

    // Extend the value of `a` to make it a multiple of `n`.
    inline uint64_t extend_multiple(uint64_t a, uint64_t n)
    {
        uint64_t r = a % n;
        return r == 0 ? a : a + (n - r);
    }

    class HashFunction
    {
      public:
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
            char data[0x40];
            size_t size;
        };

        static HashFunction* getHashFunction(Function function);

        virtual ~HashFunction()
        {}

        virtual void hash(Function function, const char* input, uint64_t length,
                          Value& output) const = 0;

        virtual bool isSupported(Function function) const = 0;

        // clang-format off
        STRINGMAP_DECLARE(HashFunctions, Function,
            { "md5",    FUNCTION_MD5    },
            { "sha1",   FUNCTION_SHA1   },
            { "sha224", FUNCTION_SHA224 },
            { "sha256", FUNCTION_SHA256 },
            { "sha384", FUNCTION_SHA384 },
            { "sha512", FUNCTION_SHA512 }
        );
        // clang-format on

      protected:
        HashFunction()
        {}
    };
} // namespace love
