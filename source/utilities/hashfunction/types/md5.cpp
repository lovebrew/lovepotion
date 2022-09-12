#include <utilities/hashfunction/types/md5.hpp>

using namespace love;

void MD5::Hash(Function function, const char* input, uint64_t length, Value& output) const
{
    if (function != FUNCTION_MD5)
        throw love::Exception("Hash function not supported by MD5 implementation");

    uint32_t a0 = 0X67452301;
    uint32_t b0 = 0XEFCDAB89;
    uint32_t c0 = 0X98BADCFE;
    uint32_t d0 = 0X10325476;

    // Do the required padding (MD5, SHA1 and SHA2 use the same padding)
    uint64_t paddedLength = length + 1; // Consider the appended bit

    if (paddedLength % 64 < 56)
        paddedLength += 56 - paddedLength % 64;

    if (paddedLength % 64 > 56)
        paddedLength += 120 - paddedLength % 64;

    uint8_t* padded = new uint8_t[paddedLength + 8];

    memcpy(padded, input, length);
    memset(padded + length, 0, paddedLength - length);

    padded[length] = 0x80;

    // Now we need the length in bits
    *((uint64_t*)&padded[paddedLength]) = length * 8;

    paddedLength += 8;

    for (uint64_t i = 0; i < paddedLength; i += 64)
    {
        uint32_t* chunk = (uint32_t*)&padded[i];

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
