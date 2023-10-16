#pragma once

#include <utilities/hashfunction/hashfunction.hpp>

namespace love
{
    class SHA1 : public HashFunction
    {
      public:
        bool IsSupported(Function function) const override
        {
            return function == FUNCTION_SHA1;
        };

        void Hash(Function function, const char* input, uint64_t length,
                  Value& output) const override;
    };
} // namespace love
