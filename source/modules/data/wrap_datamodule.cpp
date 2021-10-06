#include "modules/data/wrap_datamodule.h"

using namespace love;

#define instance() (Module::GetInstance<DataModule>(Module::M_DATA))

love::data::ContainerType Wrap_DataModule::CheckContainerType(lua_State* L, int index)
{
    const char* string             = luaL_checkstring(L, index);
    love::data::ContainerType type = love::data::CONTAINER_STRING;

    if (!DataModule::GetConstant(string, type))
        Luax::EnumError(L, "container type", DataModule::GetConstants(type), string);

    return type;
}

int Wrap_DataModule::NewByteData(lua_State* L)
{
    ByteData* byteData = nullptr;

    if (Luax::IsType(L, 1, Data::type))
    {
        Data* data = Wrap_Data::CheckData(L, 1);

        if (data->GetSize() > std::numeric_limits<lua_Integer>::max())
            return luaL_error(L, "Data's size is too large!");

        lua_Integer offset = luaL_optnumber(L, 2, 0);

        if (offset < 0)
            return luaL_error(L, "Offset argument must not be negative.");

        lua_Integer size = luaL_optinteger(L, 3, data->GetSize() - offset);

        if (size <= 0)
            return luaL_error(L, "Size argument must be greater than zero.");
        else if ((size_t)(offset + size) > data->GetSize())
            return luaL_error(L,
                              "Offset and size arguments must fit within the given Data's size.");

        const char* bytes = (const char*)data->GetData() + offset;

        Luax::CatchException(L, [&]() { byteData = instance()->NewByteData(bytes, (size_t)size); });
    }
    else if (lua_type(L, 1) == LUA_TSTRING)
    {
        size_t size      = 0;
        const char* data = luaL_checklstring(L, 1, &size);

        Luax::CatchException(L, [&]() { byteData = instance()->NewByteData(data, size); });
    }
    else
    {
        lua_Integer size = luaL_checkinteger(L, 1);

        if (size <= 0)
            return luaL_error(L, "Data size must be a positive number.");

        Luax::CatchException(L, [&]() { byteData = instance()->NewByteData((size_t)size); });
    }

    Luax::PushType(L, byteData);
    byteData->Release();

    return 1;
}

int Wrap_DataModule::Compress(lua_State* L)
{
    love::data::ContainerType type = Wrap_DataModule::CheckContainerType(L, 1);

    const char* formatStr     = luaL_checkstring(L, 2);
    Compressor::Format format = Compressor::FORMAT_LZ4;

    if (!Compressor::GetConstant(formatStr, format))
        return Luax::EnumError(L, "compressed data format", Compressor::GetConstants(format),
                               formatStr);

    int level            = (int)luaL_optinteger(L, 4, -1);
    size_t rawSize       = 0;
    const char* rawBytes = nullptr;

    if (lua_isstring(L, 3))
        rawBytes = luaL_checklstring(L, 3, &rawSize);
    else
    {
        Data* rawData = Wrap_Data::CheckData(L, 3);

        rawSize  = rawData->GetSize();
        rawBytes = (const char*)rawData->GetData();
    }

    CompressedData* compressedData = nullptr;

    Luax::CatchException(
        L, [&]() { compressedData = data::_Compress(format, rawBytes, rawSize, level); });

    if (type == data::ContainerType::CONTAINER_DATA)
        Luax::PushType(L, compressedData);
    else
        lua_pushlstring(L, (const char*)compressedData->GetData(), compressedData->GetSize());

    compressedData->Release();

    return 1;
}

int Wrap_DataModule::Decompress(lua_State* L)
{
    love::data::ContainerType type = Wrap_DataModule::CheckContainerType(L, 1);

    char* rawBytes = nullptr;
    size_t rawSize = 0;

    if (Luax::IsType(L, 2, CompressedData::type))
    {
        CompressedData* data = Wrap_CompressedData::CheckCompressedData(L, 2);
        rawSize              = data->GetDecompressedSize();

        Luax::CatchException(L, [&]() { rawBytes = data::_Decompress(data, rawSize); });
    }
    else
    {
        Compressor::Format format = Compressor::FORMAT_LZ4;
        const char* formatStr     = luaL_checkstring(L, 2);

        if (!Compressor::GetConstant(formatStr, format))
            return Luax::EnumError(L, "compressed data format", Compressor::GetConstants(format),
                                   formatStr);

        size_t compressedSize       = 0;
        const char* compressedBytes = nullptr;

        if (Luax::IsType(L, 3, Data::type))
        {
            Data* data = Luax::CheckType<Data>(L, 3);

            compressedBytes = (const char*)data->GetData();
            compressedSize  = data->GetSize();
        }
        else
            compressedBytes = luaL_checklstring(L, 3, &compressedSize);

        Luax::CatchException(L, [&]() {
            rawBytes = data::_Decompress(format, compressedBytes, compressedSize, rawSize);
        });
    }

    if (type == data::CONTAINER_DATA)
    {
        ByteData* data = nullptr;

        Luax::CatchException(L, [&]() { data = instance()->NewByteData(rawBytes, rawSize, true); });

        Luax::PushType(L, Data::type, data);
        data->Release();
    }
    else
    {
        lua_pushlstring(L, rawBytes, rawSize);
        delete[] rawBytes;
    }

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

    Luax::CatchException(
        L, [&]() { view = instance()->NewDataView(data, (size_t)offset, (size_t)size); });

    Luax::PushType(L, view);
    view->Release();

    return 1;
}

int Wrap_DataModule::Hash(lua_State* L)
{
    const char* formatStr = luaL_checkstring(L, 1);

    HashFunction::Function func;
    if (!HashFunction::GetConstant(formatStr, func))
        return Luax::EnumError(L, "hash function", HashFunction::GetConstants(func), formatStr);

    HashFunction::Value value;

    if (lua_isstring(L, 2))
    {
        size_t rawSize       = 0;
        const char* rawBytes = luaL_checklstring(L, 2, &rawSize);

        Luax::CatchException(L, [&]() { data::_Hash(func, rawBytes, rawSize, value); });
    }
    else
    {
        Data* rawData = Luax::CheckType<Data>(L, 2);

        Luax::CatchException(L, [&]() { data::_Hash(func, rawData, value); });
    }

    lua_pushlstring(L, value.data, value.size);

    return 1;
}

int Wrap_DataModule::Decode(lua_State* L)
{
    data::ContainerType ctype = Wrap_DataModule::CheckContainerType(L, 1);
    const char* formatStr     = luaL_checkstring(L, 2);

    data::EncodeFormat format;

    if (!DataModule::GetConstant(formatStr, format))
        return Luax::EnumError(L, "decode format", DataModule::GetConstants(format), formatStr);

    size_t srcLength = 0;
    const char* src  = nullptr;

    if (Luax::IsType(L, 3, Data::type))
    {
        Data* data = Luax::ToType<Data>(L, 3);

        src       = (const char*)data->GetData();
        srcLength = data->GetSize();
    }
    else
        src = luaL_checklstring(L, 3, &srcLength);

    size_t dstLength = 0;
    char* dst        = nullptr;

    Luax::CatchException(L, [&]() { dst = data::_Decode(format, src, srcLength, dstLength); });

    if (ctype == data::CONTAINER_DATA)
    {
        ByteData* data = nullptr;

        if (dst != nullptr)
            Luax::CatchException(L,
                                 [&]() { data = instance()->NewByteData(dst, dstLength, true); });
        else
            Luax::CatchException(L, [&]() { data = instance()->NewByteData(0); });

        Luax::PushType(L, Data::type, data);
        data->Release();
    }
    else
    {
        if (dst != nullptr)
            lua_pushlstring(L, dst, dstLength);
        else
            lua_pushstring(L, "");

        delete[] dst;
    }

    return 1;
}

int Wrap_DataModule::Encode(lua_State* L)
{
    data::ContainerType ctype = Wrap_DataModule::CheckContainerType(L, 1);
    const char* formatStr     = luaL_checkstring(L, 2);

    data::EncodeFormat format;

    if (!DataModule::GetConstant(formatStr, format))
        return Luax::EnumError(L, "encode format", DataModule::GetConstants(format), formatStr);

    size_t srcLength = 0;
    const char* src  = nullptr;

    if (Luax::IsType(L, 3, Data::type))
    {
        Data* data = Luax::ToType<Data>(L, 3);

        src       = (const char*)data->GetData();
        srcLength = data->GetSize();
    }
    else
        src = luaL_checklstring(L, 3, &srcLength);

    size_t lineLength = (size_t)luaL_optinteger(L, 4, 0);

    size_t dstLength = 0;
    char* dst        = nullptr;

    Luax::CatchException(
        L, [&]() { dst = data::_Encode(format, src, srcLength, dstLength, lineLength); });

    if (ctype == data::CONTAINER_DATA)
    {
        ByteData* data = nullptr;

        if (dst != nullptr)
            Luax::CatchException(L,
                                 [&]() { data = instance()->NewByteData(dst, dstLength, true); });
        else
            Luax::CatchException(L, [&]() { data = instance()->NewByteData(0); });

        Luax::PushType(L, Data::type, data);
        data->Release();
    }
    else
    {
        if (dst != nullptr)
            lua_pushlstring(L, dst, dstLength);
        else
            lua_pushstring(L, "");

        delete[] dst;
    }

    return 1;
}

int Wrap_DataModule::Pack(lua_State* L)
{
    data::ContainerType ctype = Wrap_DataModule::CheckContainerType(L, 1);
    const char* formatStr     = luaL_checkstring(L, 2);

    luaL_Buffer_53 buffer;
    lua53_str_pack(L, formatStr, 3, &buffer);

    if (ctype == data::CONTAINER_DATA)
    {
        Data* data = nullptr;

        Luax::CatchException(L, [&]() { data = instance()->NewByteData(buffer.nelems); });

        memcpy(data->GetData(), buffer.ptr, data->GetSize());

        lua53_cleanupbuffer(&buffer);

        Luax::PushType(L, Data::type, data);
        data->Release();
    }
    else
        lua53_pushresult(&buffer);

    return 1;
}

int Wrap_DataModule::Unpack(lua_State* L)
{
    const char* formatStr = luaL_checkstring(L, 1);

    const char* dataStr = nullptr;
    size_t size         = 0;

    if (Luax::IsType(L, 2, Data::type))
    {
        Data* data = Wrap_Data::CheckData(L, 2);

        dataStr = (const char*)data->GetData();
        size    = data->GetSize();
    }
    else
        dataStr = luaL_checklstring(L, 2, &size);

    return lua53_str_unpack(L, formatStr, dataStr, size, 2, 3);
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
    { 0,               0                            }
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
        Luax::CatchException(L, [&]() { instance = new DataModule(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "data";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = types;

    return Luax::RegisterModule(L, wrappedModule);
}
