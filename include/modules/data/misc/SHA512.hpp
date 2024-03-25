#pragma once

#include "common/Exception.hpp"

#include "modules/data/misc/HashFunction.hpp"

#include <cstring>

namespace love
{
    class SHA512 : public HashFunction
    {
      private:
        static constexpr uint64_t initial384[8] = {
            0xCBBB9D5DC1059ED8, 0x629A292A367CD507, 0x9159015A3070DD17, 0x152FECD8F70E5939,
            0x67332667FFC00B31, 0x8EB44A8768581511, 0xDB0C2E0D64F98FA7, 0x47B5481DBEFA4FA4,
        };

        static constexpr uint64_t initial512[8] = {
            0x6A09E667F3BCC908, 0xBB67AE8584CAA73B, 0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1,
            0x510E527FADE682D1, 0x9B05688C2B3E6C1F, 0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179,
        };

        static constexpr uint64_t constants[80] = {
            0x428A2F98D728AE22, 0x7137449123EF65CD, 0xB5C0FBCFEC4D3B2F, 0xE9B5DBA58189DBBC,
            0x3956C25BF348B538, 0x59F111F1B605D019, 0x923F82A4AF194F9B, 0xAB1C5ED5DA6D8118,
            0xD807AA98A3030242, 0x12835B0145706FBE, 0x243185BE4EE4B28C, 0x550C7DC3D5FFB4E2,
            0x72BE5D74F27B896F, 0x80DEB1FE3B1696B1, 0x9BDC06A725C71235, 0xC19BF174CF692694,
            0xE49B69C19EF14AD2, 0xEFBE4786384F25E3, 0x0FC19DC68B8CD5B5, 0x240CA1CC77AC9C65,
            0x2DE92C6F592B0275, 0x4A7484AA6EA6E483, 0x5CB0A9DCBD41FBD4, 0x76F988DA831153B5,
            0x983E5152EE66DFAB, 0xA831C66D2DB43210, 0xB00327C898FB213F, 0xBF597FC7BEEF0EE4,
            0xC6E00BF33DA88FC2, 0xD5A79147930AA725, 0x06CA6351E003826F, 0x142929670A0E6E70,
            0x27B70A8546D22FFC, 0x2E1B21385C26C926, 0x4D2C6DFC5AC42AED, 0x53380D139D95B3DF,
            0x650A73548BAF63DE, 0x766A0ABB3C77B2A8, 0x81C2C92E47EDAEE6, 0x92722C851482353B,
            0xA2BFE8A14CF10364, 0xA81A664BBC423001, 0xC24B8B70D0F89791, 0xC76C51A30654BE30,
            0xD192E819D6EF5218, 0xD69906245565A910, 0xF40E35855771202A, 0x106AA07032BBD1B8,
            0x19A4C116B8D2D0C8, 0x1E376C085141AB53, 0x2748774CDF8EEB99, 0x34B0BCB5E19B48A8,
            0x391C0CB3C5C95A63, 0x4ED8AA4AE3418ACB, 0x5B9CCA4F7763E373, 0x682E6FF3D6B2B8A3,
            0x748F82EE5DEFB2FC, 0x78A5636F43172F60, 0x84C87814A1F0AB72, 0x8CC702081A6439EC,
            0x90BEFFFA23631E28, 0xA4506CEBDE82BDE9, 0xBEF9A3F7B2C67915, 0xC67178F2E372532B,
            0xCA273ECEEA26619C, 0xD186B8C721C0C207, 0xEADA7DD6CDE0EB1E, 0xF57D4F7FEE6ED178,
            0x06F067AA72176FBA, 0x0A637DC5A2C898A6, 0x113F9804BEF90DAE, 0x1B710B35131C471B,
            0x28DB77F523047D84, 0x32CAAB7B40C72493, 0x3C9EBE0A15C9BEBC, 0x431D67C49C100D4C,
            0x4CC5D4BECB3E42B6, 0x597F299CFC657E2A, 0x5FCB6FAB3AD6FAEC, 0x6C44198C4A475817,
        };

      public:
        bool isSupported(Function function) const override
        {
            return function == FUNCTION_SHA512 || function == FUNCTION_SHA384;
        }

        void hash(Function function, const char* input, uint64_t length,
                  Value& output) const override
        {
            if (!this->isSupported(function))
                throw Exception(E_HASH_FUNCTION_NOT_SUPPORTED "SHA-384/SHA-512 implementation.");

            uint64_t intermediates[8] {};
            if (function == FUNCTION_SHA384)
                std::memcpy(intermediates, initial384, sizeof(intermediates));
            else
                std::memcpy(intermediates, initial512, sizeof(intermediates));

            uint64_t paddedLength = extend_multiple(length + 1 + 16, 128);
            uint64_t* padded      = new uint64_t[paddedLength / 8];

            std::memcpy(padded, input, length);
            std::memset(((uint8_t*)padded) + length, 0, paddedLength - length);
            *(((uint8_t*)padded) + length) = 0x80;

            uint64_t bitLength = length * 8;
            for (int index = 0; index < 8; ++index)
            {
                const uint64_t length = (bitLength >> (56 - index * 8)) & 0xFF;
                *(((uint8_t*)padded) + (paddedLength - 8 + index)) = length;
            }

            uint64_t words[80] {};

            for (uint64_t index = 0; index < paddedLength / 8; index += 16)
            {
                uint64_t* chunk = &padded[index];

                for (int j = 0; j < 16; ++j)
                {
                    char* c = (char*)&words[j];
                    c[0]    = (chunk[j] >> 56) & 0xFF;
                    c[1]    = (chunk[j] >> 48) & 0xFF;
                    c[2]    = (chunk[j] >> 40) & 0xFF;
                    c[3]    = (chunk[j] >> 32) & 0xFF;
                    c[4]    = (chunk[j] >> 24) & 0xFF;
                    c[5]    = (chunk[j] >> 16) & 0xFF;
                    c[6]    = (chunk[j] >> 8) & 0xFF;
                    c[7]    = (chunk[j] >> 0) & 0xFF;
                }

                // clang-format off
                for (int j = 16; j < 80; ++j)
                {
                    words[j] = words[j - 7] + words[j - 16];
                    words[j] += rightrot(words[j - 2], 19) ^ rightrot(words[j - 2], 61) ^ (words[j - 2] >> 6);
                    words[j] += rightrot(words[j - 15], 1) ^ rightrot(words[j - 15], 8) ^ (words[j - 15] >> 7);
                }
                // clang-format on

                uint64_t A = intermediates[0];
                uint64_t B = intermediates[1];
                uint64_t C = intermediates[2];
                uint64_t D = intermediates[3];
                uint64_t E = intermediates[4];
                uint64_t F = intermediates[5];
                uint64_t G = intermediates[6];
                uint64_t H = intermediates[7];

                // clang-format off
                for (int j = 0; j < 80; ++j)
                {
                    uint64_t temp1 = H + constants[j] + words[j];
                    temp1 += rightrot(E, 14) ^ rightrot(E, 18) ^ rightrot(E, 41);
                    temp1 += (E & F) ^ (~E & G);
                    uint64_t temp2 = rightrot(A, 28) ^ rightrot(A, 34) ^ rightrot(A, 39);
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

                intermediates[0] += A;
                intermediates[1] += B;
                intermediates[2] += C;
                intermediates[3] += D;
                intermediates[4] += E;
                intermediates[5] += F;
                intermediates[6] += G;
                intermediates[7] += H;
            }

            delete[] padded;

            int hashLength = 64;
            if (function == FUNCTION_SHA384)
                hashLength = 48;

            for (int index = 0; index < hashLength; index += 8)
            {
                output.data[index + 0] = (intermediates[index / 8] >> 56) & 0xFF;
                output.data[index + 1] = (intermediates[index / 8] >> 48) & 0xFF;
                output.data[index + 2] = (intermediates[index / 8] >> 40) & 0xFF;
                output.data[index + 3] = (intermediates[index / 8] >> 32) & 0xFF;
                output.data[index + 4] = (intermediates[index / 8] >> 24) & 0xFF;
                output.data[index + 5] = (intermediates[index / 8] >> 16) & 0xFF;
                output.data[index + 6] = (intermediates[index / 8] >> 8) & 0xFF;
                output.data[index + 7] = (intermediates[index / 8] >> 0) & 0xFF;
            }

            output.size = hashLength;
        }
    } sha512;
} // namespace love
