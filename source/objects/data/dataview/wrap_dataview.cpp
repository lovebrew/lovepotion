#include "common/luax.hpp"

#include "objects/data/dataview/wrap_dataview.hpp"

using namespace love;

DataView* Wrap_DataView::CheckDataView(lua_State* L, int index)
{
    return luax::CheckType<DataView>(L, index);
}

int Wrap_DataView::Clone(lua_State* L)
{
    DataView* self  = Wrap_DataView::CheckDataView(L, 1);
    DataView* clone = nullptr;

    luax::CatchException(L, [&]() { clone = self->Clone(); });

    luax::PushType(L, clone);
    clone->Release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone", Wrap_DataView::Clone }
};
// clang-format on

int Wrap_DataView::Register(lua_State* L)
{

    return luax::RegisterType(L, &DataView::type, Wrap_Data::functions, functions);
}
