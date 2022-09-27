#include <common/luax.hpp>

#include <common/module.hpp>
#include <common/object.hpp>
#include <common/variant.hpp>

#include <cstddef>
#include <math.h>

using namespace love;

static constexpr const char* MAIN_THREAD_KEY = "_love_mainthread";
static constexpr auto MAX_OBJECT_KEY         = 0x20000000000000ULL;

/* main stuff */

int luax::Preload(lua_State* L, lua_CFunction func, const char* name)
{
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    lua_pushcfunction(L, func);
    lua_setfield(L, -2, name);
    lua_pop(L, 2);

    return 0;
}

int luax::Require(lua_State* L, const char* name)
{
    lua_getglobal(L, "require");
    lua_pushstring(L, name);
    lua_call(L, 1, 1);

    return 1;
}

int luax::InsistGlobal(lua_State* L, const char* field)
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

lua_State* luax::InsistPinnedThread(lua_State* L)
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

lua_State* luax::GetPinnedThread(lua_State* L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, MAIN_THREAD_KEY);
    lua_State* thread = lua_tothread(L, -1);
    lua_pop(L, 1);

    return thread;
}

int luax::Insist(lua_State* L, int index, const char* key)
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

int luax::InsistLOVE(lua_State* L, const char* key)
{
    luax::InsistGlobal(L, "love");
    luax::Insist(L, -1, key);

    // replace with top stack item
    lua_replace(L, -2);

    return 1;
}

int luax::GetLOVE(lua_State* L, const char* key)
{
    lua_getglobal(L, "love");

    if (!lua_isnil(L, -1))
    {
        lua_getfield(L, -1, key);
        lua_replace(L, -2);
    }

    return 1;
}

int luax::RegisterModule(lua_State* L, const WrappedModule& wrappedModule)
{
    wrappedModule.type->Initialize();

    luax::InsistRegistry(L, Registry::REGISTRY_MODULES);

    Proxy* proxy = (love::Proxy*)lua_newuserdata(L, sizeof(Proxy));

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

    luax::InsistGlobal(L, "love");

    lua_newtable(L);

    if (!wrappedModule.functions.empty())
        luax::RegisterTypeInner(L, wrappedModule.functions);

    if (!wrappedModule.extendedFunctions.empty())
        luax::RegisterTypeInner(L, wrappedModule.extendedFunctions);

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

int luax::RegisterSearcher(lua_State* L, lua_CFunction function, int position)
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
    luax::TableInsert(L, -2, -1, position);
    lua_pop(L, 3);

    return 0;
}

void luax::RegisterTypeInit(lua_State* L, love::Type* type)
{
    type->Initialize();

    luax::GetRegistry(L, Registry::REGISTRY_OBJECTS);

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
}

void luax::RegisterTypeInner(lua_State* L, std::span<const luaL_Reg> values)
{
    for (const auto& registry : values)
    {
        lua_pushcfunction(L, registry.func);
        lua_setfield(L, -2, registry.name);
    }
}

int luax::TableInsert(lua_State* L, int tindex, int vindex, int pos)
{
    if (tindex < 0)
        tindex = lua_gettop(L) + 1 + tindex;

    if (vindex < 0)
        vindex = lua_gettop(L) + 1 + vindex;

    if (pos == -1)
    {
        lua_pushvalue(L, vindex);
        lua_rawseti(L, tindex, (int)luax::ObjectLength(L, tindex) + 1);

        return 0;
    }
    else if (pos < 0)
        pos = (int)luax::ObjectLength(L, tindex) + 1 + pos;

    for (int i = (int)luax::ObjectLength(L, tindex) + 1; i > pos; i--)
    {
        lua_rawgeti(L, tindex, i - 1);
        lua_rawseti(L, tindex, i);
    }

    lua_pushvalue(L, vindex);
    lua_rawseti(L, tindex, pos);

    return 0;
}

int luax::GetLOVEFunction(lua_State* L, const char* mod, const char* fn)
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

/* objects */

void luax::SetFunctions(lua_State* L, const luaL_Reg* reg)
{
    if (reg == nullptr)
        return;

    for (; reg->name != nullptr; reg++)
    {
        lua_pushcfunction(L, reg->func);
        lua_setfield(L, -2, reg->name);
    }
}

void luax::RawNewType(lua_State* L, love::Type& type, Object* object)
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

objectkey_t luax::ComputeObjectKey(lua_State* L, Object* object)
{
    // Compute a key to store our userdata
    const size_t minAlign = alignof(std::max_align_t);
    uintptr_t key         = (uintptr_t)object;

    if ((key & (minAlign - 1)) != 0)
    {
        luaL_error(L, "Cannot push LOVE object. Unexpected alignment (%p should be %d).", object,
                   minAlign);
    }

    static const size_t shift = (size_t)log2(alignof(std::max_align_t));
    key >>= shift;

    return key;
}

static bool isFullLightUserdataSupported(lua_State* L)
{
    static bool checked   = false;
    static bool supported = false;

    if (!checked)
    {
        lua_pushcclosure(
            L,
            [](lua_State* L) -> int {
                lua_pushlightuserdata(L, (void*)(~((size_t)0)));
                return 1;
            },
            0);

        supported = lua_pcall(L, 0, 1, 0) == 0;
        checked   = true;

        lua_pop(L, 1);
    }

    return supported;
}

void luax::PushObjectKey(lua_State* L, objectkey_t key)
{
    if (isFullLightUserdataSupported(L))
        lua_pushlightuserdata(L, (void*)key);
    else if (key > MAX_OBJECT_KEY)
        luaL_error(L, "Cannot push love object to Lua: pointer value %p is too large", key);
    else
        lua_pushnumber(L, (lua_Number)key);
}

love::Type* luax::Type(lua_State* L, int index)
{
    const char* name = luaL_checkstring(L, index);

    return love::Type::ByName(name);
}

int luax::Resume(lua_State* L, int numArgs)
{
#if LUA_VERSION_NUM >= 502
    return lua_resume(L, nullptr, nargs);
#else
    return lua_resume(L, numArgs);
#endif
}

/* registry */

int luax::InsistRegistry(lua_State* L, Registry registry)
{
    switch (registry)
    {
        case Registry::REGISTRY_MODULES:
            return luax::InsistLOVE(L, "_modules");
        case Registry::REGISTRY_OBJECTS:
            return luax::Insist(L, LUA_REGISTRYINDEX, "_loveobjects");
        default:
            return luaL_error(L, "Attempted to use invalid registry");
    }
}

int luax::GetRegistry(lua_State* L, Registry registry)
{
    switch (registry)
    {
        case Registry::REGISTRY_OBJECTS:
            lua_getfield(L, LUA_REGISTRYINDEX, "_loveobjects");
            return 1;
        case Registry::REGISTRY_MODULES:
            return luax::GetLOVE(L, "_modules");
        default:
            return luaL_error(L, "Attempted to use invalid registry");
    }
}

/* object functionality */

int luax::Equal(lua_State* L)
{
    Proxy* a = (Proxy*)lua_touserdata(L, 1);
    Proxy* b = (Proxy*)lua_touserdata(L, 2);

    bool areEqual = (a->object == b->object) && a->object != nullptr;

    lua_pushboolean(L, areEqual);

    return 1;
}

bool luax::IsType(lua_State* L, int index, love::Type& type)
{
    if (lua_type(L, index) != LUA_TUSERDATA)
        return false;

    Proxy* proxy = (Proxy*)lua_touserdata(L, index);

    if (proxy->type != nullptr)
        return proxy->type->IsA(type);
    else
        return false;
}

int luax::GarbageCollect(lua_State* L)
{
    Proxy* proxy = (Proxy*)lua_touserdata(L, 1);

    if (proxy->object != nullptr)
    {
        proxy->object->Release();
        proxy->object = nullptr;
    }

    return 0;
}

int luax::Release(lua_State* L)
{
    Proxy* proxy   = (Proxy*)lua_touserdata(L, 1);
    Object* object = proxy->object;

    if (object != nullptr)
    {
        proxy->object = nullptr;
        object->Release();

        luax::GetRegistry(L, Registry::REGISTRY_OBJECTS);

        if (lua_istable(L, -1))
        {
            objectkey_t objectKey = luax::ComputeObjectKey(L, object);

            luax::PushObjectKey(L, objectKey);
            lua_pushnil(L);
            lua_settable(L, -3);
        }

        lua_pop(L, 1);
    }

    lua_pushboolean(L, object != nullptr);

    return 1;
}

int luax::ToString(lua_State* L)
{
    Proxy* proxy         = (Proxy*)lua_touserdata(L, 1);
    const char* typeName = lua_tostring(L, lua_upvalueindex(1));

    lua_pushfstring(L, "%s: %p", typeName, proxy->object);

    return 1;
}

int luax::Type(lua_State* L)
{
    lua_pushvalue(L, lua_upvalueindex(1));

    return 1;
}

int luax::TypeOf(lua_State* L)
{
    Proxy* proxy     = (Proxy*)lua_touserdata(L, 1);
    love::Type* type = luax::Type(L, 2);

    if (!type)
        lua_pushboolean(L, 0);
    else
        lua_pushboolean(L, proxy->type->IsA(*type));

    return 1;
}

void luax::PushType(lua_State* L, love::Type& type, Object* object)
{
    if (object == nullptr)
    {
        lua_pushnil(L);
        return;
    }

    luax::GetRegistry(L, Registry::REGISTRY_OBJECTS);

    // if it doesn't exist, make it exist
    if (lua_isnoneornil(L, -1))
    {
        lua_pop(L, 1);
        return luax::RawNewType(L, type, object);
    }

    lua_Number key = luax::ComputeObjectKey(L, object);

    // push using that key
    lua_pushnumber(L, key);
    lua_gettable(L, -2);

    // if the proxy doesn't exist in registry, add it
    if (lua_type(L, -1) != LUA_TUSERDATA)
    {
        lua_pop(L, 1);
        luax::RawNewType(L, type, object);

        lua_pushnumber(L, key);
        lua_pushvalue(L, -2);

        lua_settable(L, -4);
    }

    // remove registry from the stack
    lua_remove(L, -2);
}

int luax::ConvertObject(lua_State* L, int idx, const char* mod, const char* fn)
{
    // Convert to absolute index if necessary.
    if (idx < 0 && idx > LUA_REGISTRYINDEX)
        idx += lua_gettop(L) + 1;

    // Convert string to a file.
    luax::GetLOVEFunction(L, mod, fn);
    lua_pushvalue(L, idx); // The initial argument.
    lua_call(L, 1, 2); // Call the function, one arg, one return value (plus optional errstring.)

    luax::AssertNilError(L, -2); // Make sure the function returned something.
    lua_pop(L, 1);               // Pop the second return value now that we don't need it.
    lua_replace(L, idx);         // Replace the initial argument with the new object.

    return 0;
}

int luax::ConvertObject(lua_State* L, const int idxs[], int n, const char* mod, const char* fn)
{
    luax::GetLOVEFunction(L, mod, fn);
    for (int i = 0; i < n; i++)
        lua_pushvalue(L, idxs[i]); // The arguments.

    lua_call(L, n, 2); // Call the function, n args, one return value (plus optional errstring.)

    luax::AssertNilError(L, -2); // Make sure the function returned something.
    lua_pop(L, 1);               // Pop the second return value now that we don't need it.
    if (n > 0)
        lua_replace(L, idxs[0]); // Replace the initial argument with the new object.

    return 0;
}

int luax::ConvertObject(lua_State* L, const std::vector<int>& idxs, const char* moduleName,
                        const char* function)
{
    const int* idxPtr = idxs.size() > 0 ? &idxs[0] : nullptr;

    return luax::ConvertObject(L, idxPtr, (int)idxs.size(), moduleName, function);
}

/* helpers */

size_t luax::ObjectLength(lua_State* L, int index)
{
#if LUA_VERSION_NUM == 501
    return lua_objlen(L, index);
#else
    return lua_rawlen(L, indx);
#endif
}

static Proxy* tryExtractProxy(lua_State* L, int index)
{
    Proxy* proxy = (Proxy*)lua_touserdata(L, index);

    if (proxy == nullptr || proxy->type == nullptr)
        return nullptr;

    if (dynamic_cast<Object*>(proxy->object) != nullptr)
        return proxy;

    return nullptr;
}

love::Variant luax::CheckVariant(lua_State* L, int index, bool allowUserdata,
                                 std::set<const void*>* tableSet)
{
    size_t length      = 0;
    const char* string = nullptr;

    Proxy* proxy = nullptr;

    if (index <= 0)
        index += lua_gettop(L) + 1;

    switch (lua_type(L, index))
    {
        case LUA_TBOOLEAN:
            return Variant(luax::ToBoolean(L, index));
        case LUA_TNUMBER:
            return Variant(lua_tonumber(L, index));
        case LUA_TSTRING:
        {
            string = lua_tolstring(L, index, &length);
            return Variant(string, length);
        }
        case LUA_TLIGHTUSERDATA:
            return Variant(lua_touserdata(L, index));
        case LUA_TUSERDATA:
        {
            if (!allowUserdata)
            {
                luax::TypeError(L, index, "copyable Lua value");
                return Variant();
            }

            proxy = tryExtractProxy(L, index);
            if (proxy != nullptr)
                return Variant(proxy->type, proxy->object);
            else
            {
                luax::TypeError(L, index, "love type");
                return Variant();
            }
        }
        case LUA_TNIL:
            return Variant();
        case LUA_TTABLE:
        {
            bool success = true;
            std::set<const void*> topTableSet;

            if (tableSet == nullptr)
                tableSet = &topTableSet;

            const void* tablePointer = lua_topointer(L, index);

            {
                auto result = tableSet->insert(tablePointer);
                if (!result.second)
                    throw love::Exception("Cycle detected in table!");
            }

            auto table    = new love::SharedTable();
            size_t length = luax::ObjectLength(L, -1);

            if (length > 0)
                table->pairs.reserve(length);

            lua_pushnil(L);

            while (lua_next(L, index))
            {
                // clang-format off
                table->pairs.emplace_back(
                    luax::CheckVariant(L, -2, allowUserdata, tableSet),
                    luax::CheckVariant(L, -1, allowUserdata, tableSet)
                );
                // clang-format on
            }
            lua_pop(L, 1);

            const auto& pair = table->pairs.back();
            if (pair.first.GetType() == Variant::UNKNOWN ||
                pair.second.GetType() == Variant::UNKNOWN)
            {
                success = false;
                break;
            }

            tableSet->erase(tablePointer);

            if (success)
                return Variant(table);
            else
                table->Release();
        }
        break;
    }

    return Variant::Unknown();
}

void luax::PushVariant(lua_State* L, const Variant& variant)
{
    switch (variant.GetType())
    {
        case Variant::BOOLEAN:
        {
            lua_pushboolean(L, variant.GetValue<Variant::BOOLEAN>());
            break;
        }
        case Variant::NUMBER:
        {
            lua_pushnumber(L, variant.GetValue<Variant::NUMBER>());
            break;
        }
        case Variant::STRING:
        {
            const auto string = variant.GetValue<Variant::STRING>();
            lua_pushlstring(L, string->string.get(), string->length);

            break;
        }
        case Variant::SMALLSTRING:
        {
            const auto string = variant.GetValue<Variant::SMALLSTRING>();
            lua_pushlstring(L, string.string, string.length);

            break;
        }
        case Variant::LUSERDATA:
        {
            lua_pushlightuserdata(L, variant.GetValue<Variant::LUSERDATA>());
            break;
        }
        case Variant::LOVEOBJECT:
        {
            const auto proxy = variant.GetValue<Variant::LOVEOBJECT>();
            luax::PushType(L, *proxy.type, proxy.object);

            break;
        }
        case Variant::TABLE:
        {
            std::vector<VariantPair>& table = variant.GetValue<Variant::TABLE>()->pairs;
            int tableSize                   = (int)table.size();

            lua_createtable(L, 0, tableSize);

            for (size_t index = 0; index < (size_t)tableSize; ++index)
            {
                VariantPair& keyValue = table[index];

                luax::PushVariant(L, keyValue.first);
                luax::PushVariant(L, keyValue.second);

                lua_settable(L, -3);
            }

            break;
        }
        case Variant::NIL:
        default:
        {
            lua_pushnil(L);
            break;
        }
    }
}

/* errors */

int luax::TypeError(lua_State* L, int argc, const char* name)
{
    int argType          = lua_type(L, argc);
    const char* typeName = nullptr;

    // We want to use the love type name for userdata, if possible.
    if (argType == LUA_TUSERDATA && luaL_getmetafield(L, argc, "type") != 0)
    {
        lua_pushvalue(L, argc);
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
        typeName = lua_typename(L, argType);

    const char* msg = lua_pushfstring(L, "%s expected, got %s", name, typeName);

    return luaL_argerror(L, argc, msg);
}

int luax::EnumError(lua_State* L, const char* enumName, const char* value)
{
    return luaL_error(L, "Invalid %s: %s", enumName, value);
}

int luax::IOError(lua_State* L, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    lua_pushnil(L);
    lua_pushvfstring(L, format, args);

    va_end(args);

    return 2;
}

int luax::AssertNilError(lua_State* L, int index)
{
    if (lua_isnoneornil(L, index))
    {
        if (lua_isstring(L, index + 1))
            return luaL_error(L, lua_tostring(L, index + 1));
        else
            return luaL_error(L, "assertion failed!");
    }
    return 0;
}

int luax::Traceback(lua_State* L)
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
