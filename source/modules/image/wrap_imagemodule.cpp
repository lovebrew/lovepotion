#include "modules/image/wrap_imagemodule.h"
#include "modules/image/imagemodule.h"

using namespace love;

#define instance() (Module::GetInstance<ImageModule>(Module::M_IMAGE))

int Wrap_ImageModule::NewImageData(lua_State* L)
{
    if (lua_isnumber(L, 1))
    {
        int width  = luaL_checkinteger(L, 1);
        int height = luaL_checkinteger(L, 2);

        if (width <= 0 || height <= 0)
            return luaL_error(L, "Invalid image size.");

        PixelFormat format = PIXELFORMAT_RGBA8;
        if (Luax::IsCTR())
            format = PIXELFORMAT_TEX3DS_RGBA8;

        if (!lua_isnoneornil(L, 3))
        {
            const char* formatStr = luaL_checkstring(L, 3);
            if (!ImageModule::GetConstant(formatStr, format))
                return Luax::EnumError(L, "pixel format", formatStr);
        }

        size_t numberOfBytes = 0;
        const char* bytes    = nullptr;

        if (Luax::IsType(L, 4, Data::type))
        {
            Data* data = Wrap_Data::CheckData(L, 4);

            bytes         = (const char*)data->GetData();
            numberOfBytes = data->GetSize();
        }
        else if (!lua_isnoneornil(L, 4))
            bytes = luaL_checklstring(L, 4, &numberOfBytes);

        ImageData* imageData = nullptr;
        Luax::CatchException(
            L, [&]() { imageData = instance()->NewImageData(width, height, format); });

        if (bytes)
        {
            if (numberOfBytes != imageData->GetSize())
            {
                imageData->Release();
                return luaL_error(L, "The size of the raw byte string must match the "
                                     "ImageData's actual size in bytes.");
            }
            memcpy(imageData->GetData(), bytes, imageData->GetSize());
        }

        Luax::PushType(L, imageData);
        imageData->Release();

        return 1;
    }
    else if (Wrap_Filesystem::CanGetData(L, 1))
    {
        Data* data           = Wrap_Filesystem::GetData(L, 1);
        ImageData* imageData = nullptr;

        Luax::CatchException(
            L, [&]() { imageData = instance()->NewImageData(data); },
            [&](bool) { data->Release(); });

        Luax::PushType(L, imageData);
        imageData->Release();

        return 1;
    }
    else
        return Luax::TypeErrror(L, 1, "value");

    /* should never happen */

    return 0;
}

int Wrap_ImageModule::NewCompressedData(lua_State* L)
{
    Data* data                = Wrap_Filesystem::GetData(L, 1);
    CompressedImageData* self = nullptr;

    // clang-format off
    Luax::CatchException(
        L, [&]() { self = instance()->NewCompressedData(data); },
        [&](bool) { data->Release();
    });
    // clang-format on

    Luax::PushType(L, CompressedImageData::type, self);
    self->Release();

    return 1;
}

int Wrap_ImageModule::IsCompressed(lua_State* L)
{
    Data* data = Wrap_Filesystem::GetData(L, 1);

    bool compressed = instance()->IsCompressed(data);
    data->Release();

    Luax::PushBoolean(L, compressed);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newImageData",      Wrap_ImageModule::NewImageData      },
    { "newCompressedData", Wrap_ImageModule::NewCompressedData },
    { "isCompressed",      Wrap_ImageModule::IsCompressed      },
    { 0,                   0                                   }
};

static constexpr lua_CFunction types[] =
{
    Wrap_ImageData::Register,
    Wrap_CompressedData::Register,
    nullptr
};
// clang-format on

int Wrap_ImageModule::Register(lua_State* L)
{
    ImageModule* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new ImageModule(); });
    else
        instance->Retain();

    WrappedModule module;
    module.instance  = instance;
    module.name      = "image";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return Luax::RegisterModule(L, module);
}
