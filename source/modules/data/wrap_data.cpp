#include <modules/data/wrap_data.hpp>

#include <objects/data/wrap_data.hpp>

#include <objects/data/bytedata/bytedata.hpp>
#include <objects/data/bytedata/wrap_bytedata.hpp>

#include <objects/data/compresseddata/compresseddata.hpp>
#include <objects/data/compresseddata/wrap_compresseddata.hpp>

#include <limits>
#include <objects/data/dataview/dataview.hpp>
#include <objects/data/dataview/wrap_dataview.hpp>

using namespace love;

#define instance() (Module::GetInstance<DataModule>(Module::M_DATA))

love::DataModule::ContainerType Wrap_DataModule::CheckContainerType(lua_State* L, int index)
{
    const char* string             = luaL_checkstring(L, index);
    DataModule::ContainerType type = DataModule::CONTAINER_STRING;

    if (!DataModule::GetConstant(string, type))
        luax::EnumError(L, "container type", DataModule::GetConstants(type), string);

    return type;
}

int Wrap_DataModule::Hash(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    HashFunction::Function function;

    if (!HashFunction::GetConstant(name, function))
        return luax::EnumError(L, "hash function", HashFunction::GetConstants(function), name);

    HashFunction::Value value;

    if (lua_isstring(L, 2))
    {
        size_t rawSize    = 0;
        const char* bytes = luaL_checklstring(L, 2, &rawSize);

        luax::CatchException(L, [&]() { instance()->Hash(function, bytes, rawSize, value); });
    }
    else
    {
        Data* rawData = Wrap_Data::CheckData(L, 2);

        luax::CatchException(L, [&]() { instance()->Hash(function, rawData, value); });
    }

    lua_pushlstring(L, value.data, value.size);

    return 1;
}

int Wrap_DataModule::Compress(lua_State* L)
{
    DataModule::ContainerType containerType = Wrap_DataModule::CheckContainerType(L, 1);

    const char* formatName    = luaL_checkstring(L, 2);
    Compressor::Format format = Compressor::FORMAT_LZ4;

    if (!Compressor::GetConstant(formatName, format))
        return luax::EnumError(L, "compressed data format", Compressor::GetConstants(format),
                               formatName);

    int level = luaL_optinteger(L, 4, -1);

    const char* rawBytes = nullptr;
    size_t rawSize       = 0;

    if (lua_isstring(L, 3))
        rawBytes = luaL_checklstring(L, 3, &rawSize);
    else
    {
        Data* rawData = Wrap_Data::CheckData(L, 3);

        rawSize  = rawData->GetSize();
        rawBytes = (const char*)rawData->GetData();
    }

    CompressedData* compressedData = nullptr;

    luax::CatchException(
        L, [&]() { compressedData = instance()->Compress(format, rawBytes, rawSize, level); });

    if (containerType == DataModule::CONTAINER_DATA)
        luax::PushType(L, compressedData);
    else
        lua_pushlstring(L, (const char*)compressedData->GetData(), compressedData->GetSize());

    compressedData->Release();

    return 1;
}

int Wrap_DataModule::Decompress(lua_State* L)
{
    DataModule::ContainerType containerType = Wrap_DataModule::CheckContainerType(L, 1);

    char* rawBytes = nullptr;
    size_t rawSize = 0;

    if (luax::IsType(L, 2, CompressedData::type))
    {
        CompressedData* data = Wrap_CompressedData::CheckCompressedData(L, 2);

        rawSize = data->GetDecompressedSize();

        luax::CatchException(L, [&]() { rawBytes = instance()->Decompress(data, rawSize); });
    }
    else
    {
        Compressor::Format format = Compressor::FORMAT_LZ4;
        const char* formatName    = luaL_checkstring(L, 2);

        if (!Compressor::GetConstant(formatName, format))
            return luax::EnumError(L, "compressed data format", Compressor::GetConstants(format),
                                   formatName);

        const char* compressedBytes = nullptr;
        size_t compressedSize       = 0;

        if (luax::IsType(L, 3, Data::type))
        {
            Data* data = Wrap_Data::CheckData(L, 3);

            compressedBytes = (const char*)data->GetData();
            compressedSize  = data->GetSize();
        }
        else
            compressedBytes = luaL_checklstring(L, 3, &compressedSize);

        luax::CatchException(L, [&]() {
            rawBytes = instance()->Decompress(format, compressedBytes, compressedSize, rawSize);
        });
    }

    if (containerType == DataModule::CONTAINER_DATA)
    {
        ByteData* data = nullptr;

        luax::CatchException(L, [&]() { data = instance()->NewByteData(rawBytes, rawSize, true); });

        luax::PushType(L, Data::type, data);
        data->Release();
    }
    else
    {
        lua_pushlstring(L, rawBytes, rawSize);
        delete[] rawBytes;
    }

    return 1;
}

int Wrap_DataModule::Encode(lua_State* L)
{
    DataModule::ContainerType containerType = Wrap_DataModule::CheckContainerType(L, 1);

    const char* formatName = luaL_checkstring(L, 2);
    DataModule::EncodeFormat format;

    if (!DataModule::GetConstant(formatName, format))
        return luax::EnumError(L, "encode format", DataModule::GetConstants(format), formatName);

    const char* source  = nullptr;
    size_t sourceLength = 0;

    if (luax::IsType(L, 3, Data::type))
    {
        Data* data = luax::ToType<Data>(L, 3);

        source       = (const char*)data->GetData();
        sourceLength = data->GetSize();
    }
    else
        source = luaL_checklstring(L, 3, &sourceLength);

    size_t lineLength = (size_t)luaL_optinteger(L, 4, 0);

    char* destination = nullptr;
    size_t length     = 0;

    luax::CatchException(L, [&]() {
        destination = instance()->Encode(format, source, sourceLength, length, lineLength);
    });

    if (containerType == DataModule::CONTAINER_DATA)
    {
        ByteData* data = nullptr;

        if (destination != nullptr)
            luax::CatchException(
                L, [&]() { data = instance()->NewByteData(destination, length, true); });
        else
            luax::CatchException(L, [&]() { data = instance()->NewByteData(0); });

        luax::PushType(L, Data::type, data);
        data->Release();
    }
    else
    {
        if (destination != nullptr)
            lua_pushlstring(L, destination, length);
        else
            lua_pushstring(L, "");

        delete[] destination;
    }

    return 1;
}

int Wrap_DataModule::Decode(lua_State* L)
{
    DataModule::ContainerType containerType = Wrap_DataModule::CheckContainerType(L, 1);

    const char* formatName = luaL_checkstring(L, 2);
    DataModule::EncodeFormat format;

    if (!DataModule::GetConstant(formatName, format))
        return luax::EnumError(L, "decode format", DataModule::GetConstants(format), formatName);

    const char* source  = nullptr;
    size_t sourceLength = 0;

    if (luax::IsType(L, 3, Data::type))
    {
        Data* data = luax::ToType<Data>(L, 3);

        source       = (const char*)data->GetData();
        sourceLength = data->GetSize();
    }
    else
        source = luaL_checklstring(L, 3, &sourceLength);

    char* destination = nullptr;
    size_t length     = 0;

    luax::CatchException(
        L, [&]() { destination = instance()->Decode(format, source, sourceLength, length); });

    if (containerType == DataModule::CONTAINER_DATA)
    {
        ByteData* data = nullptr;

        if (destination != nullptr)
            luax::CatchException(
                L, [&]() { data = instance()->NewByteData(destination, length, true); });
        else
            luax::CatchException(L, [&]() { data = instance()->NewByteData(0); });

        luax::PushType(L, Data::type, data);
        data->Release();
    }
    else
    {
        if (destination != nullptr)
            lua_pushlstring(L, destination, length);
        else
            lua_pushstring(L, "");

        delete[] destination;
    }

    return 1;
}

int Wrap_DataModule::NewByteData(lua_State* L)
{
    ByteData* byteData = nullptr;

    if (luax::IsType(L, 1, Data::type))
    {
        Data* data = Wrap_Data::CheckData(L, 1);

        if (data->GetSize() > std::numeric_limits<lua_Integer>::max())
            return luaL_error(L, "Data's size it too large!");

        lua_Integer offset = luaL_optinteger(L, 2, 0);

        if (offset < 0)
            return luaL_error(L, "Offset argument must not be negative.");

        lua_Integer size = luaL_optinteger(L, 3, data->GetSize() - offset);

        if (size <= 0)
            return luaL_error(L, "Size argument must be greater than zero.");
        else if ((size_t)(offset + size) > data->GetSize())
            return luaL_error(L,
                              "Offset and size arguments must fit within the given Data's size.");

        const char* bytes = (const char*)data->GetData() + offset;
        luax::CatchException(L, [&]() { byteData = instance()->NewByteData(bytes, (size_t)size); });
    }
    else if (lua_type(L, 1) == LUA_TSTRING)
    {
        size_t size      = 0;
        const char* data = luaL_checklstring(L, 1, &size);

        luax::CatchException(L, [&]() { byteData = instance()->NewByteData(data, size); });
    }
    else
    {
        lua_Integer size = luaL_checkinteger(L, 1);

        if (size <= 0)
            return luaL_error(L, "Data size must be a positive number.");

        luax::CatchException(L, [&]() { byteData = instance()->NewByteData((size_t)size); });
    }

    luax::PushType(L, byteData);
    byteData->Release();

    return 1;
}

int Wrap_DataModule::NewDataView(lua_State* L)
{
    Data* data = Wrap_Data::CheckData(L, 1);

    lua_Integer offset = luaL_checkinteger(L, 2);
    lua_Integer size   = luaL_checkinteger(L, 3);

    if (offset < 0 || size < 0)
        return luaL_error(L, "DataView offset and size must not be negative.");

    DataView* view = nullptr;

    luax::CatchException(
        L, [&]() { view = instance()->NewDataView(data, (size_t)offset, (size_t)size); });

    luax::PushType(L, view);
    view->Release();

    return 1;
}

int Wrap_DataModule::Pack(lua_State* L)
{
    DataModule::ContainerType containerType = Wrap_DataModule::CheckContainerType(L, 1);
    const char* format                      = luaL_checkstring(L, 2);

    luaL_Buffer_53 buffer {};
    lua53_str_pack(L, format, 3, &buffer);

    if (containerType == DataModule::CONTAINER_DATA)
    {
        Data* data = nullptr;

        luax::CatchException(L, [&]() { data = instance()->NewByteData(buffer.nelems); });
        memcpy(data->GetData(), buffer.ptr, data->GetSize());

        lua53_cleanupbuffer(&buffer);

        luax::PushType(L, Data::type, data);
    }
    else
        lua53_pushresult(&buffer);

    return 1;
}

int Wrap_DataModule::Unpack(lua_State* L)
{
    const char* format = luaL_checkstring(L, 1);

    const char* result = nullptr;
    size_t resultSize  = 0;

    if (luax::IsType(L, 2, Data::type))
    {
        Data* data = Wrap_Data::CheckData(L, 2);

        result     = (const char*)data->GetData();
        resultSize = data->GetSize();
    }
    else
        result = luaL_checklstring(L, 2, &resultSize);

    return lua53_str_unpack(L, format, result, resultSize, 2, 3);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "compress",      Wrap_DataModule::Compress    },
    { "decode",        Wrap_DataModule::Decode      },
    { "decompress",    Wrap_DataModule::Decompress  },
    { "encode",        Wrap_DataModule::Encode      },
    { "getPackedSize", lua53_str_packsize           },
    { "hash",          Wrap_DataModule::Hash        },
    { "newByteData",   Wrap_DataModule::NewByteData },
    { "newDataView",   Wrap_DataModule::NewDataView },
    { "pack",          Wrap_DataModule::Pack        },
    { "unpack",        Wrap_DataModule::Unpack      },
};

static constexpr lua_CFunction types[] =
{
    Wrap_Data::Register,
    Wrap_ByteData::Register,
    Wrap_CompressedData::Register,
    Wrap_DataView::Register,
    nullptr
};
// clang-format on

int Wrap_DataModule::Register(lua_State* L)
{
    DataModule* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new DataModule(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "data";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = types;

    return luax::RegisterModule(L, wrappedModule);
}
