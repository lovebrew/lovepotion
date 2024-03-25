#include "modules/data/wrap_DataView.hpp"
#include "modules/data/wrap_Data.hpp"

using namespace love;

int Wrap_DataView::clone(lua_State* L)
{
    auto* self      = luax_checkdataview(L, 1);
    DataView* clone = nullptr;

    luax_catchexcept(L, [&] { clone = new DataView(*self); });

    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone", Wrap_DataView::clone }
};
// clang-format on

namespace love
{
    DataView* luax_checkdataview(lua_State* L, int index)
    {
        return luax_checktype<DataView>(L, index);
    }

    int open_dataview(lua_State* L)
    {
        return luax_register_type(L, &DataView::type, Wrap_Data::functions, functions);
    }
} // namespace love
