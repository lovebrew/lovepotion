#include <modules/data/wrap_data.hpp>

#include <objects/data/bytedata/wrap_bytedata.hpp>
#include <objects/data/compresseddata/wrap_compresseddata.hpp>
#include <objects/data/dataview/wrap_dataview.hpp>

#include <utilities/base64.hpp>
#include <utilities/bytes.hpp>

using namespace love;

CompressedData* DataModule::Compress(Compressor::Format format, const char* rawBytes,
                                     size_t rawSize, int level)
{
    Compressor* compressor = Compressor::GetCompressor(format);

    if (compressor == nullptr)
        throw love::Exception("Invalid compression format.");

    size_t size      = 0;
    char* compressed = compressor->Compress(format, rawBytes, rawSize, level, size);

    CompressedData* data = nullptr;

    try
    {
        data = new CompressedData(format, compressed, rawSize, true);
    }
    catch (love::Exception&)
    {
        delete[] compressed;
        throw;
    }

    return data;
}

char* DataModule::Decode(EncodeFormat format, const char* source, size_t sourceLength,
                         size_t& destinationLength)
{
    switch (format)
    {
        case ENCODE_HEX:
            return (char*)love::HexToBytes(source, sourceLength, destinationLength);
        default:
        case ENCODE_BASE64:
            return love::Base64Decode(source, sourceLength, destinationLength);
    }
}

char* DataModule::Decompress(Compressor::Format format, const char* compressedBytes,
                             size_t compressedSize, size_t& rawSize)
{
    Compressor* compressor = Compressor::GetCompressor(format);

    if (compressor == nullptr)
        throw love::Exception("Invalid compression format.");

    return compressor->Decompress(format, compressedBytes, compressedSize, rawSize);
}

char* DataModule::Decompress(CompressedData* data, size_t& decompressedsize)
{
    size_t rawSize = data->GetDecompressedSize();
    char* rawBytes =
        this->Decompress(data->GetFormat(), (const char*)data->GetData(), data->GetSize(), rawSize);

    decompressedsize = rawSize;

    return rawBytes;
}

char* DataModule::Encode(EncodeFormat format, const char* source, size_t sourceLength,
                         size_t& destinationLength, size_t lineLength)
{
    switch (format)
    {
        case ENCODE_HEX:
            return love::BytesToHex((const uint8_t*)source, sourceLength, destinationLength);
        default:
        case ENCODE_BASE64:
            return love::Base64Encode(source, sourceLength, lineLength, destinationLength);
    }
}

void DataModule::Hash(HashFunction::Function func, const char* input, uint64_t size,
                      HashFunction::Value& output)
{
    HashFunction* function = HashFunction::GetHashFunction(func);

    if (function == nullptr)
        throw love::Exception("Invalid hash function.");

    function->Hash(func, input, size, output);
}

std::string DataModule::Hash(HashFunction::Function function, const char* input, uint64_t size)
{
    HashFunction::Value output;
    this->Hash(function, input, size, output);

    return std::string(output.data, output.size);
}

void DataModule::Hash(HashFunction::Function function, Data* input, HashFunction::Value& output)
{
    this->Hash(function, (const char*)input->GetData(), input->GetSize(), output);
}

std::string DataModule::Hash(HashFunction::Function function, Data* input)
{
    return this->Hash(function, (const char*)input->GetData(), input->GetSize());
}

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

// clang-format off
static constexpr lua_CFunction types[] =
{
    Wrap_DataModule::Register,
    Wrap_ByteData::Register,
    Wrap_CompressedData::Register,
    Wrap_DataView::Register,
    nullptr
};
// clang-format on
