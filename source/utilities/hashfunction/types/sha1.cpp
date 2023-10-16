#include <utilities/hashfunction/types/sha1.hpp>

using namespace love;

void SHA1::Hash(Function function, const char* input, uint64_t length, Value& output) const
{
    if (function != FUNCTION_SHA1)
        throw love::Exception("Hash function not supported by SHA1 implementation");

    uint32_t intermediate[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };

    // Do the required padding (MD5, SHA1 and SHA2 use the same padding)
    uint64_t paddedLength = length + 1; // Consider the appended bit

    if (paddedLength % 64 < 56)
        paddedLength += 56 - paddedLength % 64;

    if (paddedLength % 64 > 56)
        paddedLength += 120 - paddedLength % 64;

    uint8_t* padded = new uint8_t[paddedLength + 8];

    std::memcpy(padded, input, length);
    std::memset(padded + length, 0, paddedLength - length);

    padded[length] = 0x80;

    // Now we need the length in bits (big endian)
    length *= 8;
    for (int i = 0; i < 8; ++i, ++paddedLength)
        padded[paddedLength] = (length >> (56 - i * 8)) & 0xFF;

    // Allocate our extended words
    uint32_t words[80];

    for (uint64_t i = 0; i < paddedLength; i += 64)
    {
        uint32_t* chunk = (uint32_t*)&padded[i];

        for (int j = 0; j < 16; j++)
        {
            char* c = (char*)&words[j];
            c[0]    = (chunk[j] >> 24) & 0xFF;
            c[1]    = (chunk[j] >> 16) & 0xFF;
            c[2]    = (chunk[j] >> 8) & 0xFF;
            c[3]    = (chunk[j] >> 0) & 0xFF;
        }

        for (int j = 16; j < 80; j++)
            words[j] = leftrot(words[j - 3] ^ words[j - 8] ^ words[j - 14] ^ words[j - 16], 1);

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

    for (int i = 0; i < 20; i += 4)
    {
        output.data[i + 0] = (intermediate[i / 4] >> 24) & 0xFF;
        output.data[i + 1] = (intermediate[i / 4] >> 16) & 0xFF;
        output.data[i + 2] = (intermediate[i / 4] >> 8) & 0xFF;
        output.data[i + 3] = (intermediate[i / 4] >> 0) & 0xFF;
    }

    output.size = 20;
}
