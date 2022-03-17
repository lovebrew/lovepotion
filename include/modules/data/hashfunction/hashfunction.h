#pragma once

#include "common/data.h"
#include "common/exception.h"

#include <vector>

namespace love
{
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
        static std::vector<const char*> GetConstants(Function);

      protected:
        HashFunction()
        {}
    };
} // namespace love
