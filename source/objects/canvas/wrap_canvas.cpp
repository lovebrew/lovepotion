#include "objects/canvas/wrap_canvas.h"

#include "modules/graphics/graphics.h"

using namespace love;

int Wrap_Canvas::RenderTo(lua_State* L)
{
    Canvas* self = Wrap_Canvas::CheckCanvas(L, 1);
    int start    = 2;

    luaL_checktype(L, start, LUA_TFUNCTION);

    auto graphics = Module::GetInstance<Graphics>(Module::M_GRAPHICS);

    if (graphics)
    {
        Luax::CatchException(L, [&]() { graphics->SetCanvas(self); });

        lua_settop(L, 2); // make sure the function is on top of the stack
        int status = lua_pcall(L, 0, 0, 0);

        graphics->SetCanvas(nullptr);

        if (status != 0)
            return lua_error(L);
    }

    return 0;
}

Canvas* Wrap_Canvas::CheckCanvas(lua_State* L, int index)
{
    return Luax::CheckType<Canvas>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "renderTo", Wrap_Canvas::RenderTo },
    { 0, 0 }
};
// clang-format on

int Wrap_Canvas::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Canvas::type, Wrap_Texture::functions, functions, nullptr);
}
