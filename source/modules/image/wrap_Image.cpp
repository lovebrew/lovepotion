#include "common/Data.hpp"

#include "modules/image/wrap_CompressedImageData.hpp"
#include "modules/image/wrap_Image.hpp"
#include "modules/image/wrap_ImageData.hpp"

#include "modules/data/wrap_Data.hpp"
#include "modules/filesystem/wrap_Filesystem.hpp"

using namespace love;

#define instance() (Module::getInstance<Image>(Module::M_IMAGE))

int Wrap_Image::newImageData(lua_State* L)
{
    if (lua_isnumber(L, 1))
    {
        int width  = luaL_checkinteger(L, 1);
        int height = luaL_checkinteger(L, 2);

        if (width <= 0 || height <= 0)
            return luaL_error(L, "Invalid image size.");

        auto format = PIXELFORMAT_RGBA8_UNORM;

        if (!lua_isnoneornil(L, 3))
        {
            const char* formatName = luaL_checkstring(L, 3);
            if (!love::getConstant(formatName, format))
                return luax_enumerror(L, "pixel format", formatName);
        }

        size_t numBytes   = 0;
        const char* bytes = nullptr;

        if (luax_istype(L, 4, Data::type))
        {
            auto* data = luax_checkdata(L, 4);
            bytes      = (const char*)data->getData();
            numBytes   = data->getSize();
        }
        else if (!lua_isnoneornil(L, 4))
            bytes = luaL_checklstring(L, 4, &numBytes);

        ImageData* imageData = nullptr;
        luax_catchexcept(L, [&]() { imageData = instance()->newImageData(width, height, format); });

        if (bytes)
        {
            if (numBytes != imageData->getSize())
            {
                imageData->release();
                return luaL_error(L, "Data size does not match ImageData size.");
            }

            std::memcpy(imageData->getData(), bytes, imageData->getSize());
        }

        luax_pushtype(L, imageData);
        imageData->release();

        return 1;
    }
    else if (luax_cangetdata(L, 1))
    {
        auto* data           = luax_getdata(L, 1);
        ImageData* imageData = nullptr;

        // clang-format off
        luax_catchexcept(L,
            [&]() { imageData = instance()->newImageData(data); },
            [&](bool) { data->release(); }
        );
        // clang-format on

        luax_pushtype(L, imageData);
        imageData->release();

        return 1;
    }
    else
        return luax_typeerror(L, 1, "value");
}

int Wrap_Image::newCompressedData(lua_State* L)
{
    auto* data = luax_getdata(L, 1);

    CompressedImageData* compressedData = nullptr;

    // clang-format off
    luax_catchexcept(L,
        [&]() { compressedData = instance()->newCompressedData(data); },
        [&](bool) { data->release(); }
    );
    // clang-format on

    luax_pushtype(L, CompressedImageData::type, compressedData);
    compressedData->release();

    return 1;
}

int Wrap_Image::isCompressed(lua_State* L)
{
    Data* data = luax_getdata(L, 1);

    bool compressed = instance()->isCompressed(data);
    data->release();

    luax_pushboolean(L, compressed);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newImageData",      Wrap_Image::newImageData      },
    { "newCompressedData", Wrap_Image::newCompressedData },
    { "isCompressed",      Wrap_Image::isCompressed      }
};

static constexpr lua_CFunction types[] =
{
    love::open_imagedata,
    love::open_compressedimagedata
};
// clang-format on

int Wrap_Image::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new Image(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "image";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax_register_module(L, module);
}
