#include <common/luax.hpp>
#include <common/reference.hpp>

using namespace love;

static constexpr const char REFERENCE_TABLE_NAME[] = "love-references";

Reference::Reference() : pinnedState(nullptr), index(LUA_REFNIL)
{}

Reference::Reference(lua_State* L) : pinnedState(nullptr), index(LUA_REFNIL)
{
    this->Create(L);
}

Reference::~Reference()
{
    this->UnReference();
}

void Reference::Create(lua_State* L)
{
    this->UnReference();

    this->pinnedState = luax::GetPinnedThread(L);
    luax::Insist(L, LUA_REGISTRYINDEX, REFERENCE_TABLE_NAME);
    lua_insert(L, -2);
    this->index = luaL_ref(L, -2);
    lua_pop(L, 1);
}

void Reference::UnReference()
{
    if (this->index != LUA_REFNIL)
    {
        luax::Insist(this->pinnedState, LUA_REGISTRYINDEX, REFERENCE_TABLE_NAME);
        luaL_unref(this->pinnedState, -1, this->index);
        lua_pop(this->pinnedState, 1);
        this->index = LUA_REFNIL;
    }
}

void Reference::Push(lua_State* L) const
{
    if (this->index != LUA_REFNIL)
    {
        luax::Insist(L, LUA_REGISTRYINDEX, REFERENCE_TABLE_NAME);
        lua_rawgeti(L, -1, this->index);
        lua_remove(L, -2);
    }
    else
        lua_pushnil(L);
}