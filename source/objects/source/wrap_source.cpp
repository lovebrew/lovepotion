#include <objects/source/wrap_source.hpp>

using namespace love;
using Source = love::Source<Console::Which>;

::Source* Wrap_Source::CheckSource(lua_State* L, int index)
{
    return luax::CheckType<::Source>(L, index);
}
