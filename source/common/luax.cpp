#include "common/luax.h"

#include "common/module.h"
#include "common/stringmap.h"
#include "objects/object.h"

// C++
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <sstream>

using namespace love;

int Luax::TableInsert(lua_State* L, int tindex, int vindex, int pos)
{
    if (tindex < 0)
        tindex = lua_gettop(L) + 1 + tindex;

    if (vindex < 0)
        vindex = lua_gettop(L) + 1 + vindex;

    if (pos == -1)
    {
        lua_pushvalue(L, vindex);
        lua_rawseti(L, tindex, (int)lua_objlen(L, tindex) + 1);

        return 0;
    }
    else if (pos < 0)
        pos = (int)lua_objlen(L, tindex) + 1 + pos;

    for (int i = (int)lua_objlen(L, tindex) + 1; i > pos; i--)
    {
        lua_rawgeti(L, tindex, i - 1);
        lua_rawseti(L, tindex, i);
    }

    lua_pushvalue(L, vindex);
    lua_rawseti(L, tindex, pos);

    return 0;
}

int Luax::RegisterSearcher(lua_State* L, lua_CFunction function, int position)
{
    // Add the package loader to the package.loaders table.
    lua_getglobal(L, "package");

    if (lua_isnil(L, -1))
        return luaL_error(L, "Can't register searcher: package table does not exist.");

    lua_getfield(L, -1, "loaders");

    // Lua 5.2 renamed package.loaders to package.searchers.
    if (lua_isnil(L, -1))
    {
        lua_pop(L, 1);
        lua_getfield(L, -1, "searchers");
    }

    if (lua_isnil(L, -1))
        return luaL_error(L, "Can't register searcher: package.loaders table does not exist.");

    lua_pushcfunction(L, function);
    Luax::TableInsert(L, -2, -1, position);
    lua_pop(L, 3);

    return 0;
}

/*
** @func Preload
** Preloads a Lua C function <func> into package.preload
** with <name>. See the Lua 5.1 manual for more details:
** https://www.lua.org/manual/5.1/manual.html#pdf-package.preload
*/
int Luax::Preload(lua_State* L, lua_CFunction func, const char* name)
{
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    lua_pushcfunction(L, func);
    lua_setfield(L, -2, name);
    lua_pop(L, 2);

    return 0;
}

int Luax::Resume(lua_State* L, int nargs)
{
#if LUA_VERSION_NUM >= 502
    return lua_resume(L, nullptr, nargs);
#else
    return lua_resume(L, nargs);
#endif
}

bool Luax::BoolFlag(lua_State* L, int table_index, const char* key, bool defaultValue)
{
    lua_getfield(L, table_index, key);

    bool retval;
    if (lua_isnoneornil(L, -1))
        retval = defaultValue;
    else
        retval = lua_toboolean(L, -1) != 0;

    lua_pop(L, 1);
    return retval;
}

int Luax::IntFlag(lua_State* L, int table_index, const char* key, int defaultValue)
{
    lua_getfield(L, table_index, key);

    int retval;
    if (!lua_isnumber(L, -1))
        retval = defaultValue;
    else
        retval = (int)lua_tointeger(L, -1);

    lua_pop(L, 1);
    return retval;
}

double Luax::NumberFlag(lua_State* L, int table_index, const char* key, double defaultValue)
{
    lua_getfield(L, table_index, key);

    double retval;
    if (!lua_isnumber(L, -1))
        retval = defaultValue;
    else
        retval = lua_tonumber(L, -1);

    lua_pop(L, 1);
    return retval;
}

bool Luax::ToBoolean(lua_State* L, int index)
{
    return (lua_toboolean(L, index) != 0);
}

bool Luax::CheckBoolean(lua_State* L, int index)
{
    luaL_checktype(L, index, LUA_TBOOLEAN);
    return Luax::ToBoolean(L, index);
}

void Luax::PushBoolean(lua_State* L, bool boolean)
{
    lua_pushboolean(L, boolean ? 1 : 0);
}

/*
** @func DoBuffer
** Runs a specified Lua Buffer
*/
int Luax::DoBuffer(lua_State* L, const char* buffer, size_t size, const char* name)
{
    return (luaL_loadbuffer(L, buffer, size, name) || lua_pcall(L, 0, LUA_MULTRET, 0));
}

int Luax::Require(lua_State* L, const char* name)
{
    lua_getglobal(L, "require");
    lua_pushstring(L, name);
    lua_call(L, 1, 1);

    return 1;
}

int Luax::ConvertObject(lua_State* L, int idx, const char* mod, const char* fn)
{
    // Convert to absolute index if necessary.
    if (idx < 0 && idx > LUA_REGISTRYINDEX)
        idx += lua_gettop(L) + 1;

    // Convert string to a file.
    Luax::GetLOVEFunction(L, mod, fn);
    lua_pushvalue(L, idx); // The initial argument.
    lua_call(L, 1, 2); // Call the function, one arg, one return value (plus optional errstring.)

    Luax::AssertNilError(L, -2); // Make sure the function returned something.
    lua_pop(L, 1);               // Pop the second return value now that we don't need it.
    lua_replace(L, idx);         // Replace the initial argument with the new object.

    return 0;
}

int Luax::ConvertObject(lua_State* L, const int idxs[], int n, const char* mod, const char* fn)
{
    Luax::GetLOVEFunction(L, mod, fn);
    for (int i = 0; i < n; i++)
        lua_pushvalue(L, idxs[i]); // The arguments.

    lua_call(L, n, 2); // Call the function, n args, one return value (plus optional errstring.)

    Luax::AssertNilError(L, -2); // Make sure the function returned something.
    lua_pop(L, 1);               // Pop the second return value now that we don't need it.
    if (n > 0)
        lua_replace(L, idxs[0]); // Replace the initial argument with the new object.

    return 0;
}

int Luax::ConvertObject(lua_State* L, const std::vector<int>& idxs, const char* moduleName,
                        const char* function)
{
    const int* idxPtr = idxs.size() > 0 ? &idxs[0] : nullptr;

    return Luax::ConvertObject(L, idxPtr, (int)idxs.size(), moduleName, function);
}

/*
** @func InsistRegistry
** Creates the @registry if necessary, pushing it to the stack
*/
int Luax::InsistRegistry(lua_State* L, Registry registry)
{
    switch (registry)
    {
        case Registry::REGISTRY_MODULES:
            return Luax::InsistLove(L, "_modules");
        case Registry::REGISTRY_OBJECTS:
            return Luax::Insist(L, LUA_REGISTRYINDEX, "_loveobjects");
        default:
            return luaL_error(L, "Attempted to use invalid registry");
    }
}

/*
** @func GetRegistry
** Gets the field for the Lua registry index <registry>.
** This is necessary to store userdata and push it later.
*/
int Luax::GetRegistry(lua_State* L, Registry registry)
{
    switch (registry)
    {
        case Registry::REGISTRY_OBJECTS:
            lua_getfield(L, LUA_REGISTRYINDEX, "_loveobjects");
            return 1;
        case Registry::REGISTRY_MODULES:
            return Luax::GetLove(L, "_modules");
        default:
            return luaL_error(L, "Attempted to use invalid registry");
    }
}

int Luax::AssertNilError(lua_State* L, int idx)
{
    if (lua_isnoneornil(L, idx))
    {
        if (lua_isstring(L, idx + 1))
            return luaL_error(L, lua_tostring(L, idx + 1));
        else
            return luaL_error(L, "assertion failed!");
    }
    return 0;
}

int Luax::GetLOVEFunction(lua_State* L, const char* mod, const char* fn)
{
    lua_getglobal(L, "love");
    if (lua_isnil(L, -1))
        return luaL_error(L, "Could not find global love!");

    lua_getfield(L, -1, mod);
    if (lua_isnil(L, -1))
        return luaL_error(L, "Could not find love.%s!", mod);

    lua_getfield(L, -1, fn);
    if (lua_isnil(L, -1))
        return luaL_error(L, "Could not find love.%s.%s!", mod, fn);

    lua_remove(L, -2); // remove mod
    lua_remove(L, -2); // remove fn

    return 0;
}

/*
** @func Insist
** Make sure @index with @key exists
*/
int Luax::Insist(lua_State* L, int index, const char* key)
{
    if (index < 0 && index > LUA_REGISTRYINDEX)
        index += lua_gettop(L) + 1;

    lua_getfield(L, index, key);

    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, index, key);
    }

    return 1;
}

static const char* MAIN_THREAD_KEY = "_love_mainthread";

lua_State* Luax::InsistPinnedThread(lua_State* L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, MAIN_THREAD_KEY);

    if (lua_isnoneornil(L, -1))
    {
        lua_pop(L, 1);
        lua_pushthread(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, LUA_REGISTRYINDEX, MAIN_THREAD_KEY);
    }

    lua_State* thread = lua_tothread(L, -1);
    lua_pop(L, 1);

    return thread;
}

lua_State* Luax::GetPinnedThread(lua_State* L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, MAIN_THREAD_KEY);
    lua_State* thread = lua_tothread(L, -1);
    lua_pop(L, 1);

    return thread;
}

int Luax::InsistGlobal(lua_State* L, const char* field)
{
    lua_getglobal(L, field);

    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1); // Pop the non-table.
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setglobal(L, field);
    }

    return 1;
}

int Luax::InsistLove(lua_State* L, const char* key)
{
    Luax::InsistGlobal(L, "love");
    Luax::Insist(L, -1, key);

    // replace with top stack item
    lua_replace(L, -2);

    return 1;
}

int Luax::GetLove(lua_State* L, const char* key)
{
    lua_getglobal(L, "love");

    if (!lua_isnil(L, -1))
    {
        lua_getfield(L, -1, key);
        lua_replace(L, -2);
    }

    return 1;
}

int Luax::RegisterModule(lua_State* L, const WrappedModule& wrappedModule)
{
    wrappedModule.type->Init();

    Luax::InsistRegistry(L, Registry::REGISTRY_MODULES);

    Proxy* proxy = (Proxy*)lua_newuserdata(L, sizeof(Proxy));

    proxy->object = wrappedModule.instance;
    proxy->type   = wrappedModule.type;

    luaL_newmetatable(L, wrappedModule.instance->GetName());
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, GarbageCollect);
    lua_setfield(L, -2, "__gc");

    lua_setmetatable(L, -2);
    lua_setfield(L, -2, wrappedModule.name);
    lua_pop(L, 1);

    Luax::InsistGlobal(L, "love");

    lua_newtable(L);

    if (wrappedModule.functions != nullptr)
        Luax::SetFunctions(L, wrappedModule.functions);

    if (wrappedModule.types != nullptr)
    {
        for (const lua_CFunction* func = wrappedModule.types; *func != nullptr; func++)
            (*func)(L);
    }

    lua_pushvalue(L, -1);
    lua_setfield(L, -3, wrappedModule.name);
    lua_remove(L, -2);

    Module::RegisterInstance(wrappedModule.instance);

    return 1;
}

void Luax::SetFunctions(lua_State* L, const luaL_Reg* reg)
{
    if (reg == nullptr)
        return;

    for (; reg->name != nullptr; reg++)
    {
        lua_pushcfunction(L, reg->func);
        lua_setfield(L, -2, reg->name);
    }
}

int Luax::GarbageCollect(lua_State* L)
{
    Proxy* proxy = (Proxy*)lua_touserdata(L, 1);

    if (proxy->object != nullptr)
    {
        proxy->object->Release();
        proxy->object = nullptr;
    }

    return 0;
}

int Luax::Type(lua_State* L)
{
    lua_pushvalue(L, lua_upvalueindex(1));

    return 1;
}

int Luax::ToString(lua_State* L)
{
    Proxy* proxy         = (Proxy*)lua_touserdata(L, 1);
    const char* typeName = lua_tostring(L, lua_upvalueindex(1));

    lua_pushfstring(L, "%s: %p", typeName, proxy->object);

    return 1;
}

love::Type* Luax::Type(lua_State* L, int index)
{
    const char* name = luaL_checkstring(L, index);
    return love::Type::ByName(name);
}

bool Luax::IsType(lua_State* L, int index, love::Type& type)
{
    if (lua_type(L, index) != LUA_TUSERDATA)
        return false;

    Proxy* proxy = (Proxy*)lua_touserdata(L, index);

    if (proxy->type != nullptr)
        return proxy->type->IsA(type);
    else
        return false;
}

int Luax::TypeOf(lua_State* L)
{
    Proxy* proxy     = (Proxy*)lua_touserdata(L, 1);
    love::Type* type = Luax::Type(L, 2);

    if (!type)
        lua_pushboolean(L, 0);
    else
        lua_pushboolean(L, proxy->type->IsA(*type));

    return 1;
}

int Luax::Equal(lua_State* L)
{
    Proxy* a = (Proxy*)lua_touserdata(L, 1);
    Proxy* b = (Proxy*)lua_touserdata(L, 2);

    bool areEqual = (a->object == b->object) && a->object != nullptr;

    lua_pushboolean(L, areEqual);

    return 1;
}

int Luax::Release(lua_State* L)
{
    Proxy* proxy   = (Proxy*)lua_touserdata(L, 1);
    Object* object = proxy->object;

    if (object != nullptr)
    {
        proxy->object = nullptr;
        object->Release();

        Luax::GetRegistry(L, Registry::REGISTRY_OBJECTS);

        if (lua_istable(L, -1))
        {
            lua_Number key = Luax::ComputeObjectKey(L, object);
            lua_pushnumber(L, key);
            lua_pushnil(L);
            lua_settable(L, -3);
        }

        lua_pop(L, 1);
    }

    lua_pushboolean(L, object != nullptr);

    return 1;
}

int Luax::RegisterType(lua_State* L, love::Type* type, ...)
{
    type->Init();

    Luax::GetRegistry(L, Registry::REGISTRY_OBJECTS);

    if (!lua_istable(L, -1))
    {
        lua_newtable(L);
        lua_replace(L, -2);

        lua_newtable(L);

        lua_pushliteral(L, "v");
        lua_setfield(L, -2, "__mode");

        lua_setmetatable(L, -2);

        lua_setfield(L, LUA_REGISTRYINDEX, "_loveobjects");
    }
    else
        lua_pop(L, 1);

    luaL_newmetatable(L, type->GetName());

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, GarbageCollect);
    lua_setfield(L, -2, "__gc");

    lua_pushcfunction(L, Equal);
    lua_setfield(L, -2, "__eq");

    lua_pushstring(L, type->GetName());
    lua_pushcclosure(L, ToString, 1);
    lua_setfield(L, -2, "__tostring");

    lua_pushstring(L, type->GetName());
    lua_pushcclosure(L, Type, 1);
    lua_setfield(L, -2, "type");

    lua_pushcfunction(L, TypeOf);
    lua_setfield(L, -2, "typeOf");

    lua_pushcfunction(L, Release);
    lua_setfield(L, -2, "release");

    va_list funcs;
    va_start(funcs, type);

    for (const luaL_Reg* f = va_arg(funcs, const luaL_Reg*); f; f = va_arg(funcs, const luaL_Reg*))
        Luax::SetFunctions(L, f);

    va_end(funcs);

    lua_pop(L, 1);

    return 0;
}

void Luax::GetTypeMetaTable(lua_State* L, const love::Type& type)
{
    const char* name = type.GetName();
    lua_getfield(L, LUA_REGISTRYINDEX, name);
}

void Luax::RunWrapper(lua_State* L, const char* filedata, size_t length, const char* filename,
                      const love::Type& type)
{
    Luax::GetTypeMetaTable(L, type);

    if (lua_istable(L, -1))
    {
        luaL_loadbuffer(L, filedata, length, filename);
        lua_pushvalue(L, -2);
        lua_pushnil(L);
        lua_call(L, 2, 0);
    }

    lua_pop(L, 1);
}

void Luax::RawNewType(lua_State* L, love::Type& type, Object* object)
{
    Proxy* userdata = (Proxy*)lua_newuserdata(L, sizeof(Proxy));

    object->Retain();

    userdata->object = object;
    userdata->type   = &type;

    const char* name = type.GetName();
    luaL_newmetatable(L, name);

    lua_getfield(L, -1, "__gc");
    bool hasGC = !lua_isnoneornil(L, -1);

    lua_pop(L, 1);

    if (!hasGC)
    {
        lua_pushcfunction(L, GarbageCollect);
        lua_setfield(L, -2, "__gc");
    }

    lua_setmetatable(L, -2);
}

lua_Number Luax::ComputeObjectKey(lua_State* L, Object* object)
{
    // Compute a key to store our userdata
    const size_t minAlign = alignof(std::max_align_t);
    uintptr_t key         = (uintptr_t)object;

    if ((key & (minAlign - 1)) != 0)
        luaL_error(L, "Cannot push LOVE object. Unexpected alignment (%p should be %d).", object,
                   minAlign);

    static const size_t shift = (size_t)log2(alignof(std::max_align_t));
    key >>= shift;

    if (key > MAX_LUAOBJ_KEY)
        luaL_error(L, "Cannot push LOVE object. Pointer value %p is too large.", object);

    return (lua_Number)key;
}

int Luax::IOError(lua_State* L, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    lua_pushnil(L);
    lua_pushvfstring(L, format, args);

    va_end(args);

    return 2;
}

int Luax::EnumError(lua_State* L, const char* enumName, const char* value)
{
    return luaL_error(L, "Invalid %s: %s", enumName, value);
}

int Luax::EnumError(lua_State* L, const char* enumName, const std::vector<const char*>& values,
                    const char* value)
{
    std::string enums;
    bool first = true;

    for (auto& item : values)
    {
        enums += (first ? "'" : ", '") + std::string(item) + "'";
        first = false;
    }

    return luaL_error(L, "Invalid %s '%s', expected one of: %s", enumName, value, enums.c_str());
}

void Luax::PushType(lua_State* L, love::Type& type, Object* object)
{
    if (object == nullptr)
    {
        lua_pushnil(L);
        return;
    }

    Luax::GetRegistry(L, Registry::REGISTRY_OBJECTS);

    // if it doesn't exist, make it exist
    if (lua_isnoneornil(L, -1))
    {
        lua_pop(L, 1);
        return Luax::RawNewType(L, type, object);
    }

    lua_Number key = Luax::ComputeObjectKey(L, object);

    // push using that key
    lua_pushnumber(L, key);
    lua_gettable(L, -2);

    // if the proxy doesn't exist in registry, add it
    if (lua_type(L, -1) != LUA_TUSERDATA)
    {
        lua_pop(L, 1);
        Luax::RawNewType(L, type, object);

        lua_pushnumber(L, key);
        lua_pushvalue(L, -2);

        lua_settable(L, -4);
    }

    // remove registry from the stack
    lua_remove(L, -2);
}

int Luax::TypeErrror(lua_State* L, int narg, const char* name)
{
    int argtype          = lua_type(L, narg);
    const char* typeName = nullptr;

    // We want to use the love type name for userdata, if possible.
    if (argtype == LUA_TUSERDATA && luaL_getmetafield(L, narg, "type") != 0)
    {
        lua_pushvalue(L, narg);
        if (lua_pcall(L, 1, 1, 0) == 0 && lua_type(L, -1) == LUA_TSTRING)
        {
            typeName = lua_tostring(L, -1);

            // Non-love userdata might have a type metamethod which doesn't
            // describe its type properly, so we only use it for love types.
            if (!love::Type::ByName(typeName))
                typeName = nullptr;
        }
    }

    if (typeName == nullptr)
        typeName = lua_typename(L, argtype);

    const char* msg = lua_pushfstring(L, "%s expected, got %s", name, typeName);
    return luaL_argerror(L, narg, msg);
}

Reference* Luax::RefIf(lua_State* L, int type)
{
    Reference* r = nullptr;

    // Create a reference only if the test succeeds.
    if (lua_type(L, -1) == type)
        r = new Reference(L);
    else // Pop the value manually if it fails (done by Reference if it succeeds).
        lua_pop(L, 1);

    return r;
}

int Luax::Traceback(lua_State* L)
{
    if (!lua_isstring(L, 1)) // 'message' not a string?
        return 1;            // keep it intact

    lua_getglobal(L, "debug");
    if (!lua_istable(L, -1))
    {
        lua_pop(L, 1);
        return 1;
    }

    lua_getfield(L, -1, "traceback");
    if (!lua_isfunction(L, -1))
    {
        lua_pop(L, 2);
        return 1;
    }

    lua_pushvalue(L, 1);   // pass error message
    lua_pushinteger(L, 2); // skip this function and traceback
    lua_call(L, 2, 1);     // call debug.traceback

    return 1;
}
