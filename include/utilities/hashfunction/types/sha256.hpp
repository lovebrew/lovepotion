#pragma once

#include <utilities/hashfunction/hashfunction.hpp>

namespace love
{
    class SHA256 : public HashFunction
    {
      public:
        bool IsSupported(Function function) const override
        {
            return function == FUNCTION_SHA224 || function == FUNCTION_SHA256;
        };

        void Hash(Function function, const char* input, uint64_t length,
                  Value& output) const override;

      private:
        static constexpr uint32_t initial224[0x08] = {
            0XC1059ED8, 0X367CD507, 0X3070DD17, 0XF70E5939,
            0XFFC00B31, 0X68581511, 0X64F98FA7, 0XBEFA4FA4,
        };

        static constexpr uint32_t initial256[0x08] = {
            0X6A09E667, 0XBB67AE85, 0X3C6EF372, 0XA54FF53A,
            0X510E527F, 0X9B05688C, 0X1F83D9AB, 0X5BE0CD19,
        };

        static constexpr uint32_t constants[0x40] = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
            0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
            0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
            0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
            0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
            0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
            0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
            0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
            0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
        };
    };
} // namespace love
