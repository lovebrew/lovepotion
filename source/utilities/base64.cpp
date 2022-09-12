#include <common/exception.hpp>

#include <utilities/base64.hpp>

#include <limits>
#include <stdio.h>

using namespace love;

// Translation table as described in RFC1113
static constexpr const char cb64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Translation table to decode (created by Bob Trower)
static constexpr const char cd64[] =
    "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

// Encode 3 8-bit binary bytes as 4 '6-bit' characters
static void base64EncodeBlock(char in[3], char out[4], int len)
{
    out[0] = (char)cb64[(int)((in[0] & 0xfc) >> 2)];
    out[1] = (char)cb64[(int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4))];
    out[2] = (char)(len > 1 ? cb64[(int)(((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6))]
                            : '=');
    out[3] = (char)(len > 2 ? cb64[(int)(in[2] & 0x3f)] : '=');
}

char* love::Base64Encode(const char* source, size_t sourceLength, size_t lineLength,
                         size_t& destinationLength)
{
    if (lineLength == 0)
        lineLength = std::numeric_limits<size_t>::max();

    size_t blocksout      = 0;
    size_t sourcePosition = 0;

    size_t adjustment = (sourceLength % 3) ? (3 - (sourceLength % 3)) : 0;
    size_t paddedlen  = ((sourceLength + adjustment) / 3) * 4;

    destinationLength = paddedlen + paddedlen / lineLength;

    if (destinationLength == 0)
        return nullptr;

    char* destination = nullptr;

    try
    {
        destination = new char[destinationLength + 1];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    size_t destinationPosition = 0;

    while (sourcePosition < sourceLength)
    {
        char in[3]  = { 0 };
        char out[4] = { 0 };

        int length = 0;

        for (int i = 0; i < 3; i++)
        {
            if (sourcePosition >= sourceLength)
                break;

            in[i] = source[sourcePosition++];
            length++;
        }

        if (length > 0)
        {
            base64EncodeBlock(in, out, length);

            // clang-format off
            for (int i = 0; i < 4 && destinationPosition < destinationLength; i++, destinationPosition++)
                destination[destinationPosition] = out[i];
            // clang-format on
            blocksout++;
        }

        if (blocksout >= lineLength / 4 || sourcePosition >= sourceLength)
        {
            if (blocksout > 0 && destinationPosition < destinationLength)
                destination[destinationPosition++] = '\n';

            blocksout = 0;
        }
    }

    destination[destinationPosition] = '\0';
    return destination;
}

static void base64DecodeBlock(char in[4], char out[3])
{
    out[0] = (char)(in[0] << 2 | in[1] >> 4);
    out[1] = (char)(in[1] << 4 | in[2] >> 2);
    out[2] = (char)(((in[2] << 6) & 0xc0) | in[3]);
}

char* love::Base64Decode(const char* source, size_t sourceLength, size_t& size)
{
    size_t paddedsize = (sourceLength / 4) * 3;

    char* destination = nullptr;

    try
    {
        destination = new char[paddedsize];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    char* d = destination;

    char in[4]  = { 0 };
    char out[3] = { 0 };
    size_t i, len, sourcePosition = 0;

    while (sourcePosition <= sourceLength)
    {
        for (len = 0, i = 0; i < 4 && sourcePosition <= sourceLength; i++)
        {
            char v = 0;

            while (sourcePosition <= sourceLength && v == 0)
            {
                v = source[sourcePosition++];
                v = (char)((v < 43 || v > 122) ? 0 : cd64[v - 43]);

                if (v != 0)
                    v = (char)((v == '$') ? 0 : v - 61);
            }

            if (sourcePosition <= sourceLength)
            {
                len++;
                if (v != 0)
                    in[i] = (char)(v - 1);
            }
            else
                in[i] = 0;
        }

        if (len)
        {
            base64DecodeBlock(in, out);

            for (i = 0; i < len - 1; i++)
                *(d++) = out[i];
        }
    }

    size = (size_t)(ptrdiff_t)(d - destination);

    return destination;
}
