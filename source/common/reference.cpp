#include "common/reference.h"
#include "common/luax.h"

using namespace love;

const char* REFERENCE_TABLE_NAME = "love-references";

Reference::Reference() : pinnedState(nullptr), index(LUA_REFNIL)
{}

Reference::Reference(lua_State* L) : pinnedState(nullptr), index(LUA_REFNIL)
{
    this->Ref(L);
}

Reference::~Reference()
{
    this->UnRef();
}

void Reference::Ref(lua_State* L)
{
    this->UnRef();

    this->pinnedState = Luax::GetPinnedThread(L);
    Luax::Insist(L, LUA_REGISTRYINDEX, REFERENCE_TABLE_NAME);

    lua_insert(L, -2);
    this->index = luaL_ref(L, -2);
    lua_pop(L, 1);
}

void Reference::UnRef()
{
    if (this->index != LUA_REFNIL)
    {
        Luax::Insist(this->pinnedState, LUA_REGISTRYINDEX, REFERENCE_TABLE_NAME);
        luaL_unref(this->pinnedState, -1, this->index);
        lua_pop(this->pinnedState, 1);

        this->index = LUA_REFNIL;
    }
}

void Reference::Push(lua_State* L)
{
    if (this->index != LUA_REFNIL)
    {
        Luax::Insist(L, LUA_REGISTRYINDEX, REFERENCE_TABLE_NAME);
        lua_rawgeti(L, -1, this->index);
        lua_remove(L, -2);
    }
    else
        lua_pushnil(L);
}
