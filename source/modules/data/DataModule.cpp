#include "modules/data/DataModule.hpp"

#include "common/b64.hpp"
#include "common/int.hpp"

#include <cmath>
#include <list>

namespace
{
    static constexpr char hexChars[17] = "0123456789abcdef";

    char* bytesToHex(const uint8_t* source, size_t sourceLength, size_t& destinationLength)
    {
        destinationLength = sourceLength * 2;

        if (destinationLength == 0)
            return nullptr;

        char* destination = nullptr;

        try
        {
            destination = new char[destinationLength + 1];
        }
        catch (std::bad_alloc& e)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        for (size_t index = 0; index < sourceLength; index++)
        {
            uint8_t byte = source[index];

            destination[index * 2 + 0] = hexChars[byte >> 4];
            destination[index * 2 + 1] = hexChars[byte & 0x0F];
        }

        destination[destinationLength] = '\0';
        return destination;
    }

    uint8_t nibble(char c)
    {
        if (c >= '0' && c <= '9')
            return (uint8_t)(c - '0');

        if (c >= 'A' && c <= 'F')
            return (uint8_t)(c - 'A' + 0x0A);

        if (c >= 'a' && c <= 'f')
            return (uint8_t)(c - 'a' + 0x0A);

        return 0;
    }

    uint8_t* hexToBytes(const char* source, size_t sourceLength, size_t& destinationLength)
    {
        if (sourceLength >= 2 && source[0] == '0' && (source[1] == 'x' || source[1] == 'X'))
        {
            source += 2;
            sourceLength -= 2;
        }

        destinationLength = (sourceLength + 1) / 2;

        if (destinationLength == 0)
            return nullptr;

        uint8_t* destination = nullptr;

        try
        {
            destination = new uint8_t[destinationLength];
        }
        catch (std::bad_alloc&)
        {
            throw love::Exception(E_OUT_OF_MEMORY);
        }

        for (size_t i = 0; i < destinationLength; i++)
        {
            destination[i] = nibble(source[i * 2]) << 4;

            if (i * 2 + 1 < sourceLength)
                destination[i] |= nibble(source[i * 2 + 1]);
        }

        return destination;
    }
} // namespace

namespace love
{
    namespace data
    {
        CompressedData* compress(Compressor::Format format, const char* bytes, size_t size, int level)
        {
            auto* compressor = Compressor::getCompressor(format);

            if (compressor == nullptr)
                throw love::Exception("Invalid compression format.");

            size_t compressedSize = 0;
            auto* compressedBytes = compressor->compress(format, bytes, size, level, compressedSize);

            CompressedData* data = nullptr;

            try
            {
                data = new CompressedData(format, compressedBytes, compressedSize, size, true);
            }
            catch (love::Exception&)
            {
                delete[] compressedBytes;
                throw;
            }

            return data;
        }

        char* decompress(Compressor::Format format, const char* bytes, size_t size, size_t& rawSize)
        {
            auto* compressor = Compressor::getCompressor(format);

            if (compressor == nullptr)
                throw love::Exception("Invalid compression format.");

            return compressor->decompress(format, bytes, size, rawSize);
        }

        char* decompress(CompressedData* data, size_t& decompressedSize)
        {
            size_t rawSize = data->getDecompressedSize();

            auto* bytes =
                decompress(data->getFormat(), (const char*)data->getData(), data->getSize(), rawSize);

            decompressedSize = rawSize;
            return bytes;
        }

        char* encode(EncodeFormat format, const void* source, size_t size, size_t& destinationLength,
                     size_t lineLength)
        {
            switch (format)
            {
                default:
                case ENCODE_BASE64:
                    return b64_encode((const char*)source, size, lineLength, destinationLength);
                case ENCODE_HEX:
                    return bytesToHex((const uint8_t*)source, size, destinationLength);
            }
        }

        char* decode(EncodeFormat format, const char* source, size_t size, size_t& destinationLength)
        {
            switch (format)
            {
                default:
                case ENCODE_BASE64:
                    return b64_decode(source, size, destinationLength);
                case ENCODE_HEX:
                    return (char*)hexToBytes(source, size, destinationLength);
            }
        }

        void hash(HashFunction::Function function, const char* input, uint64_t size,
                  HashFunction::Value& output)
        {
            HashFunction* hashFunction = HashFunction::getHashFunction(function);

            if (hashFunction == nullptr)
                throw love::Exception("Invalid hash function.");

            hashFunction->hash(function, input, size, output);
        }

        void hash(HashFunction::Function function, Data* input, HashFunction::Value& output)
        {
            hash(function, (const char*)input->getData(), input->getSize(), output);
        }

        std::string hash(HashFunction::Function function, const char* input, uint64_t size)
        {
            HashFunction::Value output;
            hash(function, input, size, output);

            return std::string(output.data, output.size);
        }

        std::string hash(HashFunction::Function function, Data* input)
        {
            return hash(function, (const char*)input->getData(), input->getSize());
        }
    } // namespace data

    DataModule::DataModule() : Module(M_DATA, "love.data")
    {}

    DataView* DataModule::newDataView(Data* data, size_t offset, size_t size) const
    {
        return new DataView(data, offset, size);
    }

    ByteData* DataModule::newByteData(size_t size) const
    {
        return new ByteData(size);
    }

    ByteData* DataModule::newByteData(const void* data, size_t size) const
    {
        return new ByteData(data, size);
    }

    ByteData* DataModule::newByteData(void* data, size_t size, bool own) const
    {
        return new ByteData(data, size, own);
    }
} // namespace love
