#pragma once

#include "common/Exception.hpp"

#include "modules/data/misc/HashFunction.hpp"

#include <cstring>

namespace love
{
    class SHA256 : public HashFunction
    {
      private:
        static constexpr uint32_t initial224[8] = {
            0xC1059ED8, 0x367CD507, 0x3070DD17, 0xF70E5939,
            0xFFC00B31, 0x68581511, 0x64F98FA7, 0xBEFA4FA4,
        };

        static constexpr uint32_t initial256[8] = {
            0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
            0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19,
        };

        static constexpr uint32_t constants[64] = {
            0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4,
            0xAB1C5ED5, 0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE,
            0x9BDC06A7, 0xC19BF174, 0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F,
            0x4A7484AA, 0x5CB0A9DC, 0x76F988DA, 0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7,
            0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967, 0x27B70A85, 0x2E1B2138, 0x4D2C6DFC,
            0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85, 0xA2BFE8A1, 0xA81A664B,
            0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070, 0x19A4C116,
            0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
            0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7,
            0xC67178F2,
        };

      public:
        bool isSupported(Function function) const override
        {
            return function == FUNCTION_SHA256 || function == FUNCTION_SHA224;
        }

        void hash(Function function, const char* input, uint64_t length,
                  Value& output) const override
        {
            if (!this->isSupported(function))
                throw Exception(E_HASH_FUNCTION_NOT_SUPPORTED "SHA-224/SHA-256 implementation.");

            uint64_t paddedLength = extend_multiple(length + 1 + 8, 64);
            uint32_t* padded      = new uint32_t[paddedLength / 4];

            std::memcpy(padded, input, length);
            std::memset(((uint8_t*)padded) + length, 0, paddedLength - length);
            *(((uint8_t*)padded) + length) = 0x80;

            uint64_t bitLength = length * 8;
            for (int index = 0; index < 8; ++index)
            {
                const uint64_t length = (bitLength >> (56 - index * 8)) & 0xFF;
                *(((uint8_t*)padded) + (paddedLength - 8) + index) = length;
            }

            uint32_t intermediate[8] {};
            if (function == FUNCTION_SHA224)
                std::memcpy(intermediate, initial224, sizeof(intermediate));
            else
                std::memcpy(intermediate, initial256, sizeof(intermediate));

            uint32_t words[64] {};
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
                for (int j = 16; j < 64; j++)
                {
                    words[j]  = rightrot(words[j-2], 17) ^ rightrot(words[j-2], 19) ^ (words[j-2] >> 10);
                    words[j] += rightrot(words[j-15], 7) ^ rightrot(words[j-15], 18) ^ (words[j-15] >> 3);
                    words[j] += words[j-7] + words[j-16];
                }
                // clang-format on

                uint32_t A = intermediate[0];
                uint32_t B = intermediate[1];
                uint32_t C = intermediate[2];
                uint32_t D = intermediate[3];
                uint32_t E = intermediate[4];
                uint32_t F = intermediate[5];
                uint32_t G = intermediate[6];
                uint32_t H = intermediate[7];

                // clang-format off
                for (int j = 0; j < 64; j++)
                {
                    uint32_t temp1 = H + constants[j] + words[j];
                    temp1 += rightrot(E, 6) ^ rightrot(E, 11) ^ rightrot(E, 25);
                    temp1 += (E & F) ^ (~E & G);
                    uint32_t temp2 = rightrot(A, 2) ^ rightrot(A, 13) ^ rightrot(A, 22);
                    temp2 += (A & B) ^ (A & C) ^ (B & C);

                    H = G;
                    G = F;
                    F = E;
                    E = D + temp1;
                    D = C;
                    C = B;
                    B = A;
                    A = temp1 + temp2;
                }
                // clang-format on

                intermediate[0] += A;
                intermediate[1] += B;
                intermediate[2] += C;
                intermediate[3] += D;
                intermediate[4] += E;
                intermediate[5] += F;
                intermediate[6] += G;
                intermediate[7] += H;
            }

            delete[] padded;

            int hashLength = 32;
            if (function == FUNCTION_SHA224)
                hashLength = 28;

            for (int index = 0; index < hashLength; index += 4)
            {
                output.data[index + 0] = (intermediate[index / 4] >> 24) & 0xFF;
                output.data[index + 1] = (intermediate[index / 4] >> 16) & 0xFF;
                output.data[index + 2] = (intermediate[index / 4] >> 8) & 0xFF;
                output.data[index + 3] = (intermediate[index / 4] >> 0) & 0xFF;
            }

            output.size = hashLength;
        }
    } sha256;
} // namespace love
