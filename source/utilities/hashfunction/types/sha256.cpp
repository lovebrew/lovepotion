#include <utilities/hashfunction/types/sha256.hpp>

using namespace love;

void SHA256::Hash(Function function, const char* input, uint64_t length,
                  Value& output) const
{
    if (!IsSupported(function))
        throw love::Exception(
            "Hash function not supported by SHA-224/SHA-256 implementation");

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

    // Now we need the length in bits (big endian)
    length *= 8;
    for (int i = 0; i < 8; ++i, ++paddedLength)
        padded[paddedLength] = (length >> (56 - i * 8)) & 0xFF;

    uint32_t intermediate[8];

    if (function == FUNCTION_SHA224)
        memcpy(intermediate, initial224, sizeof(intermediate));
    else
        memcpy(intermediate, initial256, sizeof(intermediate));

    // Allocate our extended words
    uint32_t words[64];

    for (uint64_t i = 0; i < paddedLength; i += 64)
    {
        uint32_t* chunk = (uint32_t*)&padded[i];

        for (int j = 0; j < 16; j++)
        {
            char* c = (char*)&words[j];

            c[0] = (chunk[j] >> 24) & 0xFF;
            c[1] = (chunk[j] >> 16) & 0xFF;
            c[2] = (chunk[j] >> 8) & 0xFF;
            c[3] = (chunk[j] >> 0) & 0xFF;
        }

        for (int j = 16; j < 64; j++)
        {
            // clang-format off
            words[j] = rightrot(words[j - 2], 17)  ^ rightrot(words[j - 2], 19)  ^ (words[j - 2]  >> 10);

            words[j] += rightrot(words[j - 15], 7) ^ rightrot(words[j - 15], 18) ^ (words[j - 15] >> 3);
            // clang-format on

            words[j] += words[j - 7] + words[j - 16];
        }

        uint32_t A = intermediate[0];
        uint32_t B = intermediate[1];
        uint32_t C = intermediate[2];
        uint32_t D = intermediate[3];
        uint32_t E = intermediate[4];
        uint32_t F = intermediate[5];
        uint32_t G = intermediate[6];
        uint32_t H = intermediate[7];

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

    int hashlength = 32;

    if (function == FUNCTION_SHA224)
        hashlength = 28;

    for (int i = 0; i < hashlength; i += 4)
    {
        output.data[i + 0] = (intermediate[i / 4] >> 24) & 0xFF;
        output.data[i + 1] = (intermediate[i / 4] >> 16) & 0xFF;
        output.data[i + 2] = (intermediate[i / 4] >> 8) & 0xFF;
        output.data[i + 3] = (intermediate[i / 4] >> 0) & 0xFF;
    }

    output.size = hashlength;
}
