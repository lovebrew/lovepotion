#include "modules/data/datamodule.h"

#include "common/base64.h"
#include "common/bidirectionalmap.h"

namespace
{
    static const char hexchars[] = "0123456789abcdef";

    char* bytesToHex(const uint8_t* src, size_t srclen, size_t& dstlen)
    {
        dstlen = srclen * 2;

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

        for (size_t i = 0; i < srclen; i++)
        {
            uint8_t b = src[i];

            dst[i * 2 + 0] = hexchars[b >> 4];
            dst[i * 2 + 1] = hexchars[b & 0xF];
        }

        dst[dstlen] = '\0';
        return dst;
    }

    uint8_t nibble(char c)
    {
        if (c >= '0' && c <= '9')
            return (uint8_t)(c - '0');

        if (c >= 'A' && c <= 'F')
            return (uint8_t)(c - 'A' + 0x0a);

        if (c >= 'a' && c <= 'f')
            return (uint8_t)(c - 'a' + 0x0a);

        return 0;
    }

    uint8_t* hexToBytes(const char* src, size_t srclen, size_t& dstlen)
    {
        if (srclen >= 2 && src[0] == '0' && (src[1] == 'x' || src[1] == 'X'))
        {
            src += 2;
            srclen -= 2;
        }

        dstlen = (srclen + 1) / 2;

        if (dstlen == 0)
            return nullptr;

        uint8_t* dst = nullptr;

        try
        {
            dst = new uint8_t[dstlen];
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception("Out of memory.");
        }

        for (size_t i = 0; i < dstlen; i++)
        {
            dst[i] = nibble(src[i * 2]) << 4;

            if (i * 2 + 1 < srclen)
                dst[i] |= nibble(src[i * 2 + 1]);
        }

        return dst;
    }
} // namespace

namespace love::data
{
    std::string _Hash(HashFunction::Function func, Data* input)
    {
        return _Hash(func, (const char*)input->GetData(), input->GetSize());
    }

    std::string _Hash(HashFunction::Function func, const char* input, uint64_t size)
    {
        HashFunction::Value output;
        _Hash(func, input, size, output);

        return std::string(output.data, output.size);
    }

    void _Hash(HashFunction::Function func, Data* input, HashFunction::Value& output)
    {
        _Hash(func, (const char*)input->GetData(), input->GetSize(), output);
    }

    void _Hash(HashFunction::Function func, const char* input, uint64_t size,
               HashFunction::Value& output)
    {
        HashFunction* function = HashFunction::GetHashFunction(func);

        if (function == nullptr)
            throw love::Exception("Invalid hash function.");

        function->Hash(func, input, size, output);
    }

    CompressedData* _Compress(Compressor::Format format, const char* rawBytes, size_t rawSize,
                              int level)
    {
        Compressor* compressor = Compressor::GetCompressor(format);

        if (compressor == nullptr)
            throw love::Exception("Invalid compression format.");

        size_t compressedSize = 0;
        char* compressedBytes =
            compressor->Compress(format, rawBytes, rawSize, level, compressedSize);

        CompressedData* data = nullptr;

        try
        {
            data = new CompressedData(format, compressedBytes, compressedSize, rawSize, true);
        }
        catch (love::Exception&)
        {
            delete[] compressedBytes;
            throw;
        }

        return data;
    }

    char* _Decompress(Compressor::Format format, const char* compressedBytes, size_t compressedSize,
                      size_t& rawSize)
    {
        Compressor* compressor = Compressor::GetCompressor(format);

        if (compressor == nullptr)
            throw love::Exception("Invalid compression format.");

        return compressor->Decompress(format, compressedBytes, compressedSize, rawSize);
    }

    char* _Decompress(CompressedData* data, size_t& decompressedsize)
    {
        size_t rawSize = data->GetDecompressedSize();
        char* rawBytes =
            _Decompress(data->GetFormat(), (const char*)data->GetData(), data->GetSize(), rawSize);

        decompressedsize = rawSize;

        return rawBytes;
    }

    char* _Encode(EncodeFormat format, const char* src, size_t srcLength, size_t& dstLength,
                  size_t lineLength)
    {
        switch (format)
        {
            case ENCODE_HEX:
                return bytesToHex((const uint8_t*)src, srcLength, dstLength);
            default:
            case ENCODE_BASE64:
                return b64_encode(src, srcLength, lineLength, dstLength);
        }
    }

    char* _Decode(EncodeFormat format, const char* src, size_t srcLength, size_t& dstLength)
    {
        switch (format)
        {
            case ENCODE_HEX:
                return (char*)hexToBytes(src, srcLength, dstLength);
            default:
            case ENCODE_BASE64:
                return b64_decode(src, srcLength, dstLength);
        }
    }

    // clang-format off
    constexpr auto encoders = BidirectionalMap<>::Create(
        "hex",    data::EncodeFormat::ENCODE_HEX,
        "base64", data::EncodeFormat::ENCODE_BASE64
    );

    constexpr auto containers = BidirectionalMap<>::Create(
        "data",   data::ContainerType::CONTAINER_DATA,
        "string", data::ContainerType::CONTAINER_STRING
    );
    // clang-format on

} // namespace love::data

using namespace love;

ByteData* DataModule::NewByteData(size_t size)
{
    return new ByteData(size);
}

ByteData* DataModule::NewByteData(const void* data, size_t size)
{
    return new ByteData(data, size);
}

ByteData* DataModule::NewByteData(void* data, size_t size, bool own)
{
    return new ByteData(data, size, own);
}

DataView* DataModule::NewDataView(Data* data, size_t offset, size_t size)
{
    return new DataView(data, offset, size);
}

bool DataModule::GetConstant(const char* in, data::ContainerType& out)
{
    return data::containers.Find(in, out);
}

std::vector<const char*> DataModule::GetConstants(data::ContainerType)
{
    return data::containers.GetNames();
}

bool DataModule::GetConstant(const char* in, data::EncodeFormat& out)
{
    return data::encoders.Find(in, out);
}

bool DataModule::GetConstant(data::EncodeFormat in, const char*& out)
{
    return data::encoders.ReverseFind(in, out);
}

std::vector<const char*> DataModule::GetConstants(data::EncodeFormat)
{
    return data::encoders.GetNames();
}
