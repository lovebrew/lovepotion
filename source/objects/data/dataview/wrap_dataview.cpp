#include "objects/data/view/wrap_dataview.h"
#include "common/luax.h"

using namespace love;

DataView* Wrap_DataView::CheckDataView(lua_State* L, int index)
{
    return Luax::CheckType<DataView>(L, index);
}

int Wrap_DataView::Register(lua_State* L)
{
    luaL_Reg reg[] = { { 0, 0 } };

    return Luax::RegisterType(L, &DataView::type, Wrap_Data::functions, reg, nullptr);
}
