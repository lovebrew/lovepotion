#include "common/b64.hpp"
#include "common/Exception.hpp"

#include <limits>

namespace love
{
    static constexpr char cb64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    static constexpr char cd64[] =
        "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

    static void b64_encode_block(char in[3], char out[4], int len)
    {
        out[0] = (char)cb64[(int)((in[0] & 0xFC) >> 2)];
        out[1] = (char)cb64[(int)(((in[0] & 0x03) << 4) | ((in[1] & 0xF0) >> 4))];
        out[2] = (char)(len > 1 ? cb64[(int)(((in[1] & 0x0F) << 2) | ((in[2] & 0xC0) >> 6))] : '=');
        out[3] = (char)(len > 2 ? cb64[(int)(in[2] & 0x3F)] : '=');
    }

    char* b64_encode(const char* source, size_t sourceLength, size_t lineLength, size_t& dstLength)
    {
        if (lineLength == 0)
            lineLength = std::numeric_limits<size_t>::max();

        size_t blocksOut      = 0;
        size_t sourcePosition = 0;

        size_t adjustment   = (sourceLength % 3) ? (3 - (sourceLength % 3)) : 0;
        size_t paddedLength = ((sourceLength + adjustment) / 3) * 4;

        dstLength = paddedLength + paddedLength / lineLength;

        if (dstLength == 0)
            return nullptr;

        char* destination = nullptr;

        try
        {
            destination = new char[dstLength + 1];
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        size_t dstPosition = 0;

        while (sourcePosition < sourceLength)
        {
            char in[3]  = { 0 };
            char out[4] = { 0 };

            int len = 0;

            for (int i = 0; i < 3; i++)
            {
                if (sourcePosition >= sourceLength)
                    break;

                in[i] = source[sourcePosition++];
                len++;
            }

            if (len > 0)
            {
                b64_encode_block(in, out, len);

                for (int i = 0; i < 4 && dstPosition < dstLength; i++, dstPosition++)
                    destination[dstPosition] = out[i];

                blocksOut++;
            }

            if (blocksOut >= lineLength / 4 || sourcePosition >= sourceLength)
            {
                if (blocksOut > 0 && dstPosition < dstLength)
                    destination[dstPosition++] = '\n';

                blocksOut = 0;
            }
        }

        destination[dstPosition] = '\0';
        return destination;
    }

    static void b64_decode_block(char in[4], char out[3])
    {
        out[0] = (char)(in[0] << 2 | in[1] >> 4);
        out[1] = (char)(in[1] << 4 | in[2] >> 2);
        out[2] = (char)(((in[2] << 6) & 0xc0) | in[3]);
    }

    char* b64_decode(const char* source, size_t sourceLength, size_t& size)
    {
        size_t paddedSize = (sourceLength / 4) * 3;

        char* destination = nullptr;

        try
        {
            destination = new char[paddedSize];
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        auto* destCopy = destination;

        char in[4]  = { 0 };
        char out[3] = { 0 };
        size_t i, len, srcpos = 0;

        while (srcpos <= sourceLength)
        {
            for (len = 0, i = 0; i < 4 && srcpos <= sourceLength; i++)
            {
                char v = 0;

                while (srcpos <= sourceLength && v == 0)
                {
                    v = source[srcpos++];
                    v = (char)((v < 43 || v > 122) ? 0 : cd64[v - 43]);
                    if (v != 0)
                        v = (char)((v == '$') ? 0 : v - 61);
                }

                if (srcpos <= sourceLength)
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
                    *(destCopy++) = out[i];
            }
        }

        size = (size_t)(ptrdiff_t)(destCopy - destination);
        return destination;
    }
} // namespace love
