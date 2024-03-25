#pragma once

#include "common/Exception.hpp"

#include "modules/data/misc/HashFunction.hpp"

namespace love
{
    class SHA1 : public HashFunction
    {
      public:
        bool isSupported(Function function) const override
        {
            return function == FUNCTION_SHA1;
        }

        void hash(Function function, const char* input, uint64_t length,
                  Value& output) const override
        {
            if (function != FUNCTION_SHA1)
                throw Exception(E_HASH_FUNCTION_NOT_SUPPORTED "SHA1 implementation.");

            uint32_t intermediate[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476,
                                         0xC3D2E1F0 };

            uint64_t paddedLength = extend_multiple(length + 1 + 8, 64);
            uint32_t* padded      = new uint32_t[paddedLength / 4];

            std::memcpy(padded, input, length);
            std::memset(((uint8_t*)padded) + length, 0, paddedLength - length);
            *(((uint8_t*)padded) + length) = 0x80;

            uint64_t bitLength = length * 8;
            for (int index = 0; index < 8; ++index)
            {
                const uint64_t length = (bitLength >> (56 - index * 8)) & 0xFF;
                *(((uint8_t*)padded) + (paddedLength - 8 + index)) = length;
            }

            uint32_t words[80] {};
            for (uint64_t index = 0; index < paddedLength / 4; index += 16)
            {
                uint32_t* chunk = &padded[index];
                for (int j = 0; j < 16; j++)
                {
                    char* c = (char*)&words[j];
                    c[0]    = (chunk[j] >> 24) & 0xFF;
                    c[1]    = (chunk[j] >> 16) & 0xFF;
                    c[2]    = (chunk[j] >> 8) & 0xFF;
                    c[3]    = (chunk[j] >> 0) & 0xFF;
                }

                // clang-format off
                for (int j = 16; j < 80; j++)
                    words[j] = leftrot(words[j - 3] ^ words[j - 8] ^ words[j - 14] ^ words[j - 16], 1);
                // clang-format on

                uint32_t A = intermediate[0];
                uint32_t B = intermediate[1];
                uint32_t C = intermediate[2];
                uint32_t D = intermediate[3];
                uint32_t E = intermediate[4];

                for (int j = 0; j < 80; j++)
                {
                    uint32_t temp = leftrot(A, 5) + E + words[j];

                    if (j < 20)
                        temp += 0x5A827999 + ((B & C) | (~B & D));
                    else if (j < 40)
                        temp += 0x6ED9EBA1 + (B ^ C ^ D);
                    else if (j < 60)
                        temp += 0x8F1BBCDC + ((B & C) | (B & D) | (C & D));
                    else
                        temp += 0xCA62C1D6 + (B ^ C ^ D);

                    E = D;
                    D = C;
                    C = leftrot(B, 30);
                    B = A;
                    A = temp;
                }

                intermediate[0] += A;
                intermediate[1] += B;
                intermediate[2] += C;
                intermediate[3] += D;
                intermediate[4] += E;
            }

            delete[] padded;

            for (int index = 0; index < 20; index += 4)
            {
                output.data[index + 0] = (intermediate[index / 4] >> 24) & 0xFF;
                output.data[index + 1] = (intermediate[index / 4] >> 16) & 0xFF;
                output.data[index + 2] = (intermediate[index / 4] >> 8) & 0xFF;
                output.data[index + 3] = (intermediate[index / 4] >> 0) & 0xFF;
            }

            output.size = 20;
        }
    } sha1;
} // namespace love
