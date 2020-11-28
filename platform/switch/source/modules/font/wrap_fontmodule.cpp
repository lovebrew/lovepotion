#include "common/runtime.h"
#include "modules/font/wrap_fontmodule.h"

using namespace love;

#define instance() (Module::GetInstance<FontModule>(Module::M_FONT))

int Wrap_FontModule::NewRasterizer(lua_State * L)
{
    /* First or second arg is a number */
    if (lua_type(L, 1) == LUA_TNUMBER || lua_type(L, 2) == LUA_TNUMBER || lua_isnone(L, 1))
        return Wrap_FontModule::NewFontRasterizer(L);
    else if (lua_isnoneornil(L, 2))
    {
        Rasterizer * self = nullptr;
        FileData * fileData = Wrap_Filesystem::GetFileData(L, 1);

        Luax::CatchException(L,
            [&]()     { self = instance()->NewRasterizer(fileData); },
            [&](bool) { fileData->Release(); }
        );

        Luax::PushType(L, self);
        self->Release();
    }
}

int Wrap_FontModule::NewFontRasterizer(lua_State * L)
{
    Rasterizer * self = nullptr;

    TrueTypeRasterizer::Hinting hinting = TrueTypeRasterizer::HINTING_NORMAL;

    if (lua_type(L, 1) == LUA_TNUMBER || lua_isnone(L, 1))
    {
        int size = (int) luaL_optinteger(L, 1, 12);

        const char * hintstr = lua_isnoneornil(L, 2) ? nullptr : luaL_checkstring(L, 2);
        if (hintstr && !TrueTypeRasterizer::GetConstant(hintstr, hinting))
            return Luax::EnumError(L, "TrueType font hinting mode", TrueTypeRasterizer::GetConstants(hinting), hintstr);

        if (lua_isnoneornil(L, 3))
            Luax::CatchException(L, [&](){ self = instance()->NewTrueTypeRasterizer(size, hinting); });
        else
        {
            float dpiscale = (float) luaL_checknumber(L, 3);
            Luax::CatchException(L, [&](){ self = instance()->NewTrueTypeRasterizer(size, dpiscale, hinting); });
        }
    }
    else
    {
        love::Data * data = nullptr;

        if (Luax::IsType(L, 1, love::Data::type))
        {
            data = Wrap_Data::CheckData(L, 1);
            data->Retain();
        }
        else
        {
            const char * str = luaL_checkstring(L, 1);

            if (std::filesystem::path(str).extension().empty())
            {
                Font::SystemFontType type = Font::SystemFontType::TYPE_STANDARD;

                if (!Font::GetConstant(str, type))
                    return Luax::EnumError(L, "font type", Font::GetConstants(type), str);
                else
                    data = instance()->GetSystemFont(type);
            }
            else /* load font from a file, *.ttf usually */
                data = Wrap_Filesystem::GetFileData(L, 1);
        }

        int size = (int)luaL_optinteger(L, 2, 12);

        const char * hintstr = lua_isnoneornil(L, 3) ? nullptr : luaL_checkstring(L, 3);
        if (hintstr && !TrueTypeRasterizer::GetConstant(hintstr, hinting))
            return Luax::EnumError(L, "TrueType font hinting mode", TrueTypeRasterizer::GetConstants(hinting), hintstr);

        if (lua_isnoneornil(L, 4))
        {
            Luax::CatchException(L,
                [&]()     { self = instance()->NewTrueTypeRasterizer(data, size, hinting); },
                [&](bool) { data->Release(); }
            );
        }
        else
        {
            float dpiscale = (float) luaL_checknumber(L, 4);
            Luax::CatchException(L,
                [&]()     { self = instance()->NewTrueTypeRasterizer(data, size, dpiscale, hinting); },
                [&](bool) { data->Release(); }
            );
        }
    }

    Luax::PushType(L, self);
    self->Release();

    return 1;
}

int Wrap_FontModule::Register(lua_State * L)
{
    luaL_reg reg[] = {
        { "newRasterizer",  Wrap_FontModule::NewRasterizer },
        { 0, 0 }
    };

    FontModule * instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new FontModule(); });
    else
        instance->Retain();

    WrappedModule module;

    module.instance = instance;
    module.name = "font";
    module.functions = reg;
    module.type = &Module::type;
    module.types = nullptr;

    return Luax::RegisterModule(L, module);
}