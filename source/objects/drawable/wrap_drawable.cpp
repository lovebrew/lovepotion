#include "objects/drawable/wrap_drawable.h"

using namespace love;

int Wrap_Drawable::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Drawable::type, nullptr);
}
