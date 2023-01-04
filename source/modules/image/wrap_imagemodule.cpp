#include <modules/image/imagemodule.hpp>
#include <modules/image/wrap_imagemodule.hpp>

#include <modules/filesystem/wrap_filesystem.hpp>

#include <objects/data/wrap_data.hpp>
#include <objects/imagedata/wrap_imagedata.hpp>

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

        std::optional<PixelFormat> format = PIXELFORMAT_RGBA8_UNORM;

        if (!lua_isnoneornil(L, 3))
        {
            const char* formatName = luaL_checkstring(L, 3);

            if (!(format = pixelFormats.Find(formatName)))
                return luax::EnumError(L, "pixel format", pixelFormats, formatName);
        }

        size_t numBytes   = 0;
        const char* bytes = nullptr;

        if (luax::IsType(L, 4, Data::type))
        {
            auto* data = Wrap_Data::CheckData(L, 4);

            numBytes = data->GetSize();
            bytes    = (const char*)data->GetData();
        }
        else if (!lua_isnoneornil(L, 4))
            bytes = luaL_checklstring(L, 4, &numBytes);

        ImageData<Console::Which>* data = nullptr;
        luax::CatchException(L, [&]() { data = instance()->NewImageData(width, height, *format); });

        if (bytes)
        {
            if (numBytes != data->GetSize())
            {
                data->Release();
                return luaL_error(L, "The size of the raw byte string must match the ImageData's "
                                     "actual size in bytes.");
            }

            std::copy_n(bytes, data->GetSize(), (char*)data->GetData());
        }

        luax::PushType(L, data);
        data->Release();

        return 1;
    }
    else if (Wrap_Filesystem::CanGetData(L, 1))
    {
        auto* data                           = Wrap_Filesystem::GetData(L, 1);
        ImageData<Console::Which>* imageData = nullptr;

        luax::CatchException(
            L, [&]() { imageData = instance()->NewImageData(data); },
            [&](bool) { data->Release(); });

        luax::PushType(L, imageData);
        imageData->Release();

        return 1;
    }

    return luax::TypeError(L, 1, "value");
}

// To do
int Wrap_ImageModule::NewCompressedData(lua_State* L)
{
    return 0;
}

int Wrap_ImageModule::IsCompressed(lua_State* L)
{
    auto* data      = Wrap_Filesystem::GetData(L, 1);
    bool compressed = instance()->IsCompressed(data);

    data->Release();

    luax::PushBoolean(L, compressed);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newImageData",      Wrap_ImageModule::NewImageData      },
    { "newCompressedData", Wrap_ImageModule::NewCompressedData },
    { "isCompressed",      Wrap_ImageModule::IsCompressed      }
};

static constexpr lua_CFunction types[] =
{
    Wrap_ImageData::Register,
    nullptr
};
// clang-format on

int Wrap_ImageModule::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new ImageModule(); });
    else
        instance()->Retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "image";
    module.functions = functions;
    module.type      = &Module::type;
    module.types     = types;

    return luax::RegisterModule(L, module);
}
