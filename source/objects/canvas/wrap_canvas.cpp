#include "common/runtime.h"
#include "objects/canvas/wrap_canvas.h"

using namespace love;

love::Type Canvas::type("Canvas", &Texture::type);

Canvas * Wrap_Canvas::CheckCanvas(lua_State * L, int index)
{
    return Luax::CheckType<Canvas>(L, index);
}

int Wrap_Canvas::Register(lua_State * L)
{
    return Luax::RegisterType(L, &Canvas::type, Wrap_Texture::functions, nullptr);
}