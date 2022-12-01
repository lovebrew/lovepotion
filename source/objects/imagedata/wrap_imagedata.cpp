#include <objects/imagedata/wrap_imagedata.hpp>

using namespace love;
using ImageData = love::ImageData<Console::Which>;

::ImageData* Wrap_ImageData::CheckImageData(lua_State* L, int index)
{
    return luax::CheckType<::ImageData>(L, index);
}

int Wrap_ImageData::Clone(lua_State* L)
{
    auto* self         = Wrap_ImageData::CheckImageData(L, 1);
    ::ImageData* clone = nullptr;

    luax::CatchException(L, [&]() { clone = self->Clone(); });

    luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_ImageData::GetFormat(lua_State* L)
{
    auto* self         = Wrap_ImageData::CheckImageData(L, 1);
    PixelFormat format = self->GetFormat();

    std::optional<const char*> formatName;
    if (!(formatName = pixelFormats.ReverseFind(format)))
        return luaL_error(L, "Unknown pixel format.");

    lua_pushstring(L, *formatName);

    return 1;
}

int Wrap_ImageData::GetWidth(lua_State* L)
{
    auto* self = Wrap_ImageData::CheckImageData(L, 1);

    lua_pushinteger(L, self->GetWidth());

    return 1;
}

int Wrap_ImageData::GetHeight(lua_State* L)
{
    auto* self = Wrap_ImageData::CheckImageData(L, 1);

    lua_pushinteger(L, self->GetHeight());

    return 1;
}

int Wrap_ImageData::GetDimensions(lua_State* L)
{
    auto* self = Wrap_ImageData::CheckImageData(L, 1);

    lua_pushinteger(L, self->GetWidth());
    lua_pushinteger(L, self->GetHeight());

    return 2;
}

int Wrap_ImageData::GetPixel(lua_State* L)
{
    auto* self = Wrap_ImageData::CheckImageData(L, 1);

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    Color color {};
    luax::CatchException(L, [&]() { self->GetPixel(x, y, color); });

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int Wrap_ImageData::SetPixel(lua_State* L)
{
    auto* self = Wrap_ImageData::CheckImageData(L, 1);

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    int components = love::GetPixelFormatColorComponents(self->GetFormat());
    Color color {};

    if (lua_istable(L, 4))
    {
        for (int index = 1; index <= components; index++)
            lua_rawgeti(L, components, index);

        color.r = luaL_checknumber(L, -components);

        if (components > 1)
            color.g = luaL_checknumber(L, (-components) + 1);
        else if (components > 2)
            color.b = luaL_checknumber(L, (-components) + 2);
        else if (components > 3)
            color.a = luaL_optnumber(L, (-components) + 3, 1.0);

        lua_pop(L, components);
    }
    else
    {
        color.r = luaL_checknumber(L, 4);

        if (components > 1)
            color.g = luaL_checknumber(L, 5);
        else if (components > 2)
            color.b = luaL_checknumber(L, 6);
        else if (components > 3)
            color.a = luaL_optnumber(L, 7, 1.0f);
    }

    luax::CatchException(L, [&]() { self->SetPixel(x, y, color); });

    return 0;
}

// To do
int Wrap_ImageData::__MapPixelUnsafe(lua_State* L)
{
    return 0;
}

// To do
int Wrap_ImageData::Paste(lua_State* L)
{
    return 0;
}

// To do
int Wrap_ImageData::Encode(lua_State* L)
{
    return 0;
}

// To do
int Wrap_ImageData::__PerformAtomic(lua_State* L)
{
    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",         Wrap_ImageData::Clone         },
    { "getFormat",     Wrap_ImageData::GetFormat     },
    { "getWidth",      Wrap_ImageData::GetWidth      },
    { "getHeight",     Wrap_ImageData::GetHeight     },
    { "getDimensions", Wrap_ImageData::GetDimensions },
    { "getPixel",      Wrap_ImageData::GetPixel      },
    { "setPixel",      Wrap_ImageData::SetPixel      },
    { "paste",         Wrap_ImageData::Paste         },
    { "encode",        Wrap_ImageData::Encode        },
    /* spacing */
    { "_mapPixelUnsafe", Wrap_ImageData::__MapPixelUnsafe },
    { "_performAtomic",  Wrap_ImageData::__PerformAtomic  }
};
// clang-format on

int Wrap_ImageData::Register(lua_State* L)
{
    return luax::RegisterType(L, &::ImageData::type, functions);
}
