#pragma once

#include "common/Exception.hpp"

#include "modules/data/misc/HashFunction.hpp"

#include <cstring>

namespace love
{
    class MD5 : public HashFunction
    {
      private:
        static constexpr uint8_t shifts[64] = {
            7,  12, 17, 22, 7,  12, 17, 22, 7,  12, 17, 22, 7,  12, 17, 22, 5,  9,  14, 20, 5,  9,
            14, 20, 5,  9,  14, 20, 5,  9,  14, 20, 4,  11, 16, 23, 4,  11, 16, 23, 4,  11, 16, 23,
            4,  11, 16, 23, 6,  10, 15, 21, 6,  10, 15, 21, 6,  10, 15, 21, 6,  10, 15, 21,
        };

        static constexpr uint32_t constants[64] = {
            0XD76AA478, 0XE8C7B756, 0X242070DB, 0XC1BDCEEE, 0XF57C0FAF, 0X4787C62A, 0XA8304613,
            0XFD469501, 0X698098D8, 0X8B44F7AF, 0XFFFF5BB1, 0X895CD7BE, 0X6B901122, 0XFD987193,
            0XA679438E, 0X49B40821, 0XF61E2562, 0XC040B340, 0X265E5A51, 0XE9B6C7AA, 0XD62F105D,
            0X02441453, 0XD8A1E681, 0XE7D3FBC8, 0X21E1CDE6, 0XC33707D6, 0XF4D50D87, 0X455A14ED,
            0XA9E3E905, 0XFCEFA3F8, 0X676F02D9, 0X8D2A4C8A, 0XFFFA3942, 0X8771F681, 0X6D9D6122,
            0XFDE5380C, 0XA4BEEA44, 0X4BDECFA9, 0XF6BB4B60, 0XBEBFBC70, 0X289B7EC6, 0XEAA127FA,
            0XD4EF3085, 0X04881D05, 0XD9D4D039, 0XE6DB99E5, 0X1FA27CF8, 0XC4AC5665, 0XF4292244,
            0X432AFF97, 0XAB9423A7, 0XFC93A039, 0X655B59C3, 0X8F0CCC92, 0XFFEFF47D, 0X85845DD1,
            0X6FA87E4F, 0XFE2CE6E0, 0XA3014314, 0X4E0811A1, 0XF7537E82, 0XBD3AF235, 0X2AD7D2BB,
            0XEB86D391,
        };

      public:
        bool isSupported(Function function) const override
        {
            return function == FUNCTION_MD5;
        }

        void hash(Function function, const char* input, uint64_t length,
                  Value& output) const override
        {
            if (!this->isSupported(function))
                throw Exception(E_HASH_FUNCTION_NOT_SUPPORTED "MD5 implementation.");

            uint32_t a0 = 0X67452301;
            uint32_t b0 = 0XEFCDAB89;
            uint32_t c0 = 0X98BADCFE;
            uint32_t d0 = 0X10325476;

            uint64_t paddedLength = extend_multiple(length + 1 + 8, 64);
            uint32_t* padded      = new uint32_t[paddedLength / 4];

            std::memcpy(padded, input, (size_t)length);
            std::memset(((uint8_t*)padded) + length, 0, (size_t)(paddedLength - length));
            *(((uint8_t*)padded) + length) = 0x80;

            uint64_t bitLength = length * 8;
            std::memcpy(((uint8_t*)padded) + paddedLength - 8, &bitLength, 8);

            for (uint64_t i = 0; i < paddedLength / 4; i += 16)
            {
                uint32_t* chunk = &padded[i];

                uint32_t A = a0;
                uint32_t B = b0;
                uint32_t C = c0;
                uint32_t D = d0;
                uint32_t F;
                uint32_t g;

                for (int j = 0; j < 64; j++)
                {
                    if (j < 16)
                    {
                        F = (B & C) | (~B & D);
                        g = j;
                    }
                    else if (j < 32)
                    {
                        F = (D & B) | (~D & C);
                        g = (5 * j + 1) % 16;
                    }
                    else if (j < 48)
                    {
                        F = B ^ C ^ D;
                        g = (3 * j + 5) % 16;
                    }
                    else
                    {
                        F = C ^ (B | ~D);
                        g = (7 * j) % 16;
                    }

                    uint32_t temp = D;
                    D             = C;
                    C             = B;
                    B += leftrot(A + F + constants[j] + chunk[g], shifts[j]);
                    A = temp;
                }

                a0 += A;
                b0 += B;
                c0 += C;
                d0 += D;
            }

            delete[] padded;

            memcpy(&output.data[0], &a0, 4);
            memcpy(&output.data[4], &b0, 4);
            memcpy(&output.data[8], &c0, 4);
            memcpy(&output.data[12], &d0, 4);
            output.size = 16;
        }
    } md5;
} // namespace love
