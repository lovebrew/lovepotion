#include <utilities/hashfunction/types/sha512.hpp>

using namespace love;

void SHA512::Hash(Function function, const char* input, uint64_t length,
                  Value& output) const
{
    if (!IsSupported(function))
        throw love::Exception(
            "Hash function not supported by SHA-384/SHA-512 implementation");

    uint64_t intermediates[8];

    if (function == FUNCTION_SHA384)
        memcpy(intermediates, initial384, sizeof(intermediates));
    else
        memcpy(intermediates, initial512, sizeof(intermediates));

    // Do the required padding
    uint64_t paddedLength = length + 1; // Consider the appended bit

    if (paddedLength % 128 < 112)
        paddedLength += 112 - paddedLength % 128;

    if (paddedLength % 128 > 112)
        paddedLength += 240 - paddedLength % 128;

    uint8_t* padded = new uint8_t[paddedLength + 16];

    paddedLength += 8;

    memcpy(padded, input, length);
    memset(padded + length, 0, paddedLength - length);

    padded[length] = 0x80;

    // Now we need the length in bits (big endian), note we only write a 64-bit int, so
    // we have filled the first 8 bytes with zeroes
    length *= 8;
    for (int i = 0; i < 8; ++i, ++paddedLength)
        padded[paddedLength] = (length >> (56 - i * 8)) & 0xFF;

    // Allocate our extended words
    uint64_t words[80];

    for (uint64_t i = 0; i < paddedLength; i += 128)
    {
        uint64_t* chunk = (uint64_t*)&padded[i];

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
        for (int j = 16; j < 80; ++j)
        {
            words[j] = words[j - 7] + words[j - 16];

            // clang-format off
            words[j] += rightrot(words[j - 2], 19) ^ rightrot(words[j - 2], 61) ^ (words[j - 2]  >> 6);

            words[j] += rightrot(words[j - 15], 1) ^ rightrot(words[j - 15], 8) ^ (words[j - 15] >> 7);
            // clang-format on
        }

        uint64_t A = intermediates[0];
        uint64_t B = intermediates[1];
        uint64_t C = intermediates[2];
        uint64_t D = intermediates[3];
        uint64_t E = intermediates[4];
        uint64_t F = intermediates[5];
        uint64_t G = intermediates[6];
        uint64_t H = intermediates[7];

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

    int hashlength = 64;
    if (function == FUNCTION_SHA384)
        hashlength = 48;

    for (int i = 0; i < hashlength; i += 8)
    {
        output.data[i + 0] = (intermediates[i / 8] >> 56) & 0xFF;
        output.data[i + 1] = (intermediates[i / 8] >> 48) & 0xFF;
        output.data[i + 2] = (intermediates[i / 8] >> 40) & 0xFF;
        output.data[i + 3] = (intermediates[i / 8] >> 32) & 0xFF;
        output.data[i + 4] = (intermediates[i / 8] >> 24) & 0xFF;
        output.data[i + 5] = (intermediates[i / 8] >> 16) & 0xFF;
        output.data[i + 6] = (intermediates[i / 8] >> 8) & 0xFF;
        output.data[i + 7] = (intermediates[i / 8] >> 0) & 0xFF;
    }

    output.size = hashlength;
}
