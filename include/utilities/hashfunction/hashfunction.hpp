#pragma once

#include <common/data.hpp>
#include <common/exception.hpp>

#include <utilities/bidirectionalmap/smallvector.hpp>

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

        static bool GetConstant(const char* in, Function& out);

        static bool GetConstant(const Function& in, const char*& out);

        static SmallTrivialVector<const char*, 6> GetConstants(Function);

      protected:
        HashFunction()
        {}
    };
} // namespace love
