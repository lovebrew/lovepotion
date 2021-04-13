#include "common/base64.h"
#include "common/exception.h"

#include <limits>
#include <stdio.h>

using namespace love;

// Translation table as described in RFC1113
static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Translation table to decode (created by Bob Trower)
static const char cd64[] =
    "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

// Encode 3 8-bit binary bytes as 4 '6-bit' characters
static void b64_encode_block(char in[3], char out[4], int len)
{
    out[0] = (char)cb64[(int)((in[0] & 0xfc) >> 2)];
    out[1] = (char)cb64[(int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4))];
    out[2] = (char)(len > 1 ? cb64[(int)(((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6))] : '=');
    out[3] = (char)(len > 2 ? cb64[(int)(in[2] & 0x3f)] : '=');
}

char* love::b64_encode(const char* src, size_t srclen, size_t linelen, size_t& dstlen)
{
    if (linelen == 0)
        linelen = std::numeric_limits<size_t>::max();

    size_t blocksout = 0;
    size_t srcpos    = 0;

    size_t adjustment = (srclen % 3) ? (3 - (srclen % 3)) : 0;
    size_t paddedlen  = ((srclen + adjustment) / 3) * 4;

    dstlen = paddedlen + paddedlen / linelen;

    if (dstlen == 0)
        return nullptr;

    char* dst = nullptr;

    try
    {
        dst = new char[dstlen + 1];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    size_t dstpos = 0;

    while (srcpos < srclen)
    {
        char in[3]  = { 0 };
        char out[4] = { 0 };

        int len = 0;

        for (int i = 0; i < 3; i++)
        {
            if (srcpos >= srclen)
                break;

            in[i] = src[srcpos++];
            len++;
        }

        if (len > 0)
        {
            b64_encode_block(in, out, len);

            for (int i = 0; i < 4 && dstpos < dstlen; i++, dstpos++)
                dst[dstpos] = out[i];

            blocksout++;
        }

        if (blocksout >= linelen / 4 || srcpos >= srclen)
        {
            if (blocksout > 0 && dstpos < dstlen)
                dst[dstpos++] = '\n';

            blocksout = 0;
        }
    }

    dst[dstpos] = '\0';
    return dst;
}

static void b64_decode_block(char in[4], char out[3])
{
    out[0] = (char)(in[0] << 2 | in[1] >> 4);
    out[1] = (char)(in[1] << 4 | in[2] >> 2);
    out[2] = (char)(((in[2] << 6) & 0xc0) | in[3]);
}

char* love::b64_decode(const char* src, size_t srclen, size_t& size)
{
    size_t paddedsize = (srclen / 4) * 3;

    char* dst = nullptr;
    try
    {
        dst = new char[paddedsize];
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Out of memory.");
    }

    char* d = dst;

    char in[4]  = { 0 };
    char out[3] = { 0 };
    size_t i, len, srcpos = 0;

    while (srcpos <= srclen)
    {
        for (len = 0, i = 0; i < 4 && srcpos <= srclen; i++)
        {
            char v = 0;

            while (srcpos <= srclen && v == 0)
            {
                v = src[srcpos++];
                v = (char)((v < 43 || v > 122) ? 0 : cd64[v - 43]);

                if (v != 0)
                    v = (char)((v == '$') ? 0 : v - 61);
            }

            if (srcpos <= srclen)
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
            b64_decode_block(in, out);

            for (i = 0; i < len - 1; i++)
                *(d++) = out[i];
        }
    }

    size = (size_t)(ptrdiff_t)(d - dst);

    return dst;
}
