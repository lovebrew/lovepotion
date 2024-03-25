#include "common/luax.hpp"

#include "common/Module.hpp"
#include "common/Object.hpp"

#include <cmath>
#include <cstring>

#define LOVE_UNUSED(x) (void)sizeof(x)

namespace love
{
    // #region Startup

    int luax_preload(lua_State* L, lua_CFunction function, const char* name)
    {
        lua_getglobal(L, "package");
        lua_getfield(L, -1, "preload");
        lua_pushcfunction(L, function);
        lua_setfield(L, -2, name);
        lua_pop(L, 2);

        return 0;
    }

    int luax_insistglobal(lua_State* L, const char* name)
    {
        lua_getglobal(L, name);

        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_pushvalue(L, -1);
            lua_setglobal(L, name);
        }

        return 1;
    }

    lua_State* luax_insistpinnedthread(lua_State* L)
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

    lua_State* luax_getpinnedthread(lua_State* L)
    {
        lua_getfield(L, LUA_REGISTRYINDEX, MAIN_THREAD_KEY);
        lua_State* thread = lua_tothread(L, -1);
        lua_pop(L, 1);

        return thread;
    }

    int luax_insist(lua_State* L, int index, const char* name)
    {
        if (index < 0 && index > LUA_REGISTRYINDEX)
            index += lua_gettop(L) + 1;

        lua_getfield(L, index, name);

        if (!lua_istable(L, -1))
        {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_pushvalue(L, -1);
            lua_setfield(L, index, name);
        }

        return 1;
    }

    int luax_insistlove(lua_State* L, const char* name)
    {
        lua_getglobal(L, "love");
        luax_insist(L, -1, name);
        lua_replace(L, -2);

        return 1;
    }

    int luax_getlove(lua_State* L, const char* name)
    {
        lua_getglobal(L, "love");

        if (!lua_isnil(L, -1))
        {
            lua_getfield(L, -1, name);
            lua_replace(L, -2);
        }

        return 1;
    }

    int luax_insistregistry(lua_State* L, Registry registry)
    {
        switch (registry)
        {
            case REGISTRY_MODULES:
                return luax_insistlove(L, MODULES_REGISTRY_KEY);
            case REGISTRY_OBJECTS:
                return luax_insist(L, LUA_REGISTRYINDEX, OBJECTS_REGISTRY_KEY);
            default:
                return luaL_error(L, "Attempted to use invalid registry.");
        }
    }

    int luax_getregistry(lua_State* L, Registry registry)
    {
        switch (registry)
        {
            case REGISTRY_MODULES:
                return luax_getlove(L, MODULES_REGISTRY_KEY);
            case REGISTRY_OBJECTS:
                lua_getfield(L, LUA_REGISTRYINDEX, OBJECTS_REGISTRY_KEY);
                return 1;
            default:
                return luaL_error(L, "Attempted to use invalid registry.");
        }
    }

    int luax_resume(lua_State* L, int argc, int* nres)
    {
#if LUA_VERSION_NUM >= 504
        return lua_resume(L, nullptr, argc, nres);
#elif LUA_VERSION_NUM >= 502
        LOVE_UNUSED(nres);
        return lua_resume(L, nullptr, argc);
#else
        LOVE_UNUSED(nres);
        return lua_resume(L, argc);
#endif
    }

    int luax_register_searcher(lua_State* L, lua_CFunction function, int position)
    {
        lua_getglobal(L, "package");

        if (lua_isnil(L, -1))
            return luaL_error(L, "Can't register searcher: package table does not exist.");

        lua_getfield(L, -1, "loaders");

        if (lua_isnil(L, -1))
        {
            lua_pop(L, 1);
            lua_getfield(L, -1, "searchers");
        }

        if (lua_isnil(L, -1))
            return luaL_error(L, "Can't register searcher: package.loaders table does not exist.");

        lua_pushcfunction(L, function);
        luax_table_insert(L, -2, -1, position);

        lua_pop(L, 3);

        return 0;
    }

    // #endregion

    // #region Objects

    void luax_pushobjectkey(lua_State* L, ObjectKey key)
    {
        if (luax_isfulllightuserdatasupported(L))
            lua_pushlightuserdata(L, (void*)key);
        else if (key > LUAX_MAX_OBJECT_KEY)
            luaL_error(L, E_POINTER_TOO_LARGE, key);
        else
            lua_pushnumber(L, (lua_Number)key);
    }

    static ObjectKey luax_computeobjectkey(lua_State* L, Object* object)
    {
        const size_t min_align = sizeof(void*) == 8 ? alignof(std::max_align_t) : 1;
        uintptr_t key          = (uintptr_t)object;

        if ((key & (min_align - 1)) != 0)
            luaL_error(L, E_UNEXPECTED_ALIGNMENT, object, (int)min_align);

        static const size_t shift = (size_t)std::log2(min_align);

        key >>= shift;
        return (ObjectKey)key;
    }

    static int w_release(lua_State* L)
    {
        Proxy* proxy   = (Proxy*)lua_touserdata(L, 1);
        Object* object = proxy->object;

        if (object != nullptr)
        {
            proxy->object = nullptr;
            object->release();

            luax_getregistry(L, REGISTRY_OBJECTS);

            if (lua_istable(L, -1))
            {
                ObjectKey key = luax_computeobjectkey(L, object);
                luax_pushobjectkey(L, key);
                lua_pushnil(L);
                lua_settable(L, -3);
            }

            lua_pop(L, 1);
        }

        luax_pushboolean(L, object != nullptr);

        return 1;
    }

    static int w__gc(lua_State* L)
    {
        Proxy* proxy = (Proxy*)lua_touserdata(L, 1);

        if (proxy->object != nullptr)
        {
            proxy->object->release();
            proxy->object = nullptr;
        }

        return 0;
    }

    static int w__tostring(lua_State* L)
    {
        Proxy* proxy = (Proxy*)lua_touserdata(L, 1);

        const char* name = lua_tostring(L, lua_upvalueindex(1));
        lua_pushfstring(L, "%s: %p", name, proxy->object);

        return 1;
    }

    static int w_type(lua_State* L)
    {
        lua_pushvalue(L, lua_upvalueindex(1));

        return 1;
    }

    static int w_typeOf(lua_State* L)
    {
        Proxy* proxy = (Proxy*)lua_touserdata(L, 1);
        Type* type   = luax_type(L, 2);

        if (!type)
            luax_pushboolean(L, false);
        else
            luax_pushboolean(L, proxy->type->isA(*type));

        return 1;
    }

    static int w__eq(lua_State* L)
    {
        Proxy* proxy1 = (Proxy*)lua_touserdata(L, 1);
        Proxy* proxy2 = (Proxy*)lua_touserdata(L, 2);

        luax_pushboolean(L, proxy1->object == proxy2->object && proxy1->object != nullptr);

        return 1;
    }

    // #endregion

    // #region Helpers

    int luax_isfulllightuserdatasupported(lua_State* L)
    {
        static bool checked   = false;
        static bool supported = false;

        if (sizeof(void*) == 4)
            return true;

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

    Type* luax_type(lua_State* L, int index)
    {
        return Type::byName(luaL_checkstring(L, index));
    }

    int luax_typeerror(lua_State* L, int argc, const char* name)
    {
        int argtype             = lua_type(L, argc);
        const char* argTypeName = nullptr;

        if (argtype == LUA_TUSERDATA && luaL_getmetafield(L, argc, "type") != 0)
        {
            lua_pushvalue(L, argc);
            if (lua_pcall(L, 1, 1, 0) == 0 && lua_type(L, -1) == LUA_TSTRING)
            {
                argTypeName = lua_tostring(L, -1);
                if (!Type::byName(argTypeName))
                    argTypeName = nullptr;
            }
        }

        if (argTypeName == nullptr)
            argTypeName = lua_typename(L, argtype);

        const char* message = lua_pushfstring(L, "%s expected, got %s", name, argTypeName);
        return luaL_argerror(L, argc, message);
    }

    bool luax_istype(lua_State* L, int index, Type& type)
    {
        if (lua_type(L, index) != LUA_TUSERDATA)
            return false;

        Proxy* userdata = (Proxy*)lua_touserdata(L, index);

        if (userdata->type != nullptr)
            return userdata->type->isA(type);
        else
            return false;
    }

    Proxy* luax_tryextractproxy(lua_State* L, int index)
    {
        Proxy* proxy = (Proxy*)lua_touserdata(L, index);

        if (!proxy || !proxy->type)
            return nullptr;

        if (dynamic_cast<Object*>(proxy->object) != nullptr)
            return proxy;

        return nullptr;
    }

    void luax_rawnewtype(lua_State* L, Type& type, Object* object)
    {
        Proxy* proxy = (Proxy*)lua_newuserdata(L, sizeof(Proxy));
        object->retain();

        proxy->object = object;
        proxy->type   = &type;

        const char* name = type.getName();
        luaL_newmetatable(L, name);

        lua_getfield(L, -1, "__gc");
        bool has_gc = !lua_isnoneornil(L, -1);
        lua_pop(L, 1);

        if (!has_gc)
        {
            lua_pushcfunction(L, w__gc);
            lua_setfield(L, -2, "__gc");
        }

        lua_setmetatable(L, -2);
    }

    void luax_pushtype(lua_State* L, Type& type, Object* object)
    {
        if (object == nullptr)
        {
            lua_pushnil(L);
            return;
        }

        luax_getregistry(L, REGISTRY_OBJECTS);

        if (lua_isnoneornil(L, -1))
        {
            lua_pop(L, 1);
            return luax_rawnewtype(L, type, object);
        }

        ObjectKey key = luax_computeobjectkey(L, object);
        luax_pushobjectkey(L, key);

        lua_gettable(L, -2);

        if (lua_type(L, -1) != LUA_TUSERDATA)
        {
            lua_pop(L, 1);

            luax_rawnewtype(L, type, object);

            luax_pushobjectkey(L, key);
            lua_pushvalue(L, -2);

            lua_settable(L, -4);
        }

        lua_remove(L, -2);
    }

    void luax_pushvariant(lua_State* L, const Variant& variant)
    {
        const Variant::Data& data = variant.getData();

        switch (variant.getType())
        {
            case Variant::BOOLEAN:
                lua_pushboolean(L, data.boolean);
                break;
            case Variant::NUMBER:
                lua_pushnumber(L, data.number);
                break;
            case Variant::STRING:
                lua_pushlstring(L, data.string->string, data.string->length);
                break;
            case Variant::SMALLSTRING:
                lua_pushlstring(L, data.smallstring.str, data.smallstring.len);
                break;
            case Variant::LUSERDATA:
                lua_pushlightuserdata(L, data.userdata);
                break;
            case Variant::LOVEOBJECT:
                luax_pushtype(L, *data.proxy.type, data.proxy.object);
                break;
            case Variant::TABLE:
            {
                auto& table = data.table->pairs;
                int tsize   = (int)table.size();

                lua_createtable(L, 0, tsize);

                for (int i = 0; i < tsize; ++i)
                {
                    std::pair<Variant, Variant>& kv = table[i];
                    luax_pushvariant(L, kv.first);
                    luax_pushvariant(L, kv.second);
                    lua_settable(L, -3);
                }

                break;
            }
            case Variant::NIL:
            default:
                lua_pushnil(L);
                break;
        }
    }

    Variant luax_checkvariant(lua_State* L, int index, bool allowuserdata,
                              std::set<const void*>* tableSet)
    {
        size_t len;
        const char* str;
        Proxy* p = nullptr;

        if (index < 0) // Fix the stack position, we might modify it later
            index += lua_gettop(L) + 1;

        switch (lua_type(L, index))
        {
            case LUA_TBOOLEAN:
                return Variant(luax_toboolean(L, index));
            case LUA_TNUMBER:
                return Variant(lua_tonumber(L, index));
            case LUA_TSTRING:
                str = lua_tolstring(L, index, &len);
                return Variant(str, len);
            case LUA_TLIGHTUSERDATA:
                return Variant(lua_touserdata(L, index));
            case LUA_TUSERDATA:
                if (!allowuserdata)
                {
                    luax_typeerror(L, index, "copyable Lua value");
                    return Variant();
                }
                p = luax_tryextractproxy(L, index);
                if (p != nullptr)
                    return Variant(p->type, p->object);
                else
                {
                    luax_typeerror(L, index, "love type");
                    return Variant();
                }
            case LUA_TNIL:
                return Variant();
            case LUA_TTABLE:
            {
                bool success = true;
                std::set<const void*> topTableSet;

                // We can use a pointer to a stack-allocated variable because it's
                // never used after the stack-allocated variable is destroyed.
                if (tableSet == nullptr)
                    tableSet = &topTableSet;

                // Now make sure this table wasn't already serialised
                const void* tablePointer = lua_topointer(L, index);
                {
                    auto result = tableSet->insert(tablePointer);
                    if (!result.second) // insertion failed
                        throw love::Exception("Cycle detected in table");
                }

                Variant::SharedTable* table = new Variant::SharedTable();

                size_t len = luax_objlen(L, -1);
                if (len > 0)
                    table->pairs.reserve(len);

                lua_pushnil(L);

                while (lua_next(L, index))
                {
                    table->pairs.emplace_back(luax_checkvariant(L, -2, allowuserdata, tableSet),
                                              luax_checkvariant(L, -1, allowuserdata, tableSet));
                    lua_pop(L, 1);

                    const auto& p = table->pairs.back();
                    if (p.first.getType() == Variant::UNKNOWN ||
                        p.second.getType() == Variant::UNKNOWN)
                    {
                        success = false;
                        break;
                    }
                }

                // And remove the table from the set again
                tableSet->erase(tablePointer);

                if (success)
                    return Variant(table);
                else
                    table->release();
            }
            break;
        }

        return Variant::unknown();
    }

    bool luax_toboolean(lua_State* L, int index)
    {
        return (lua_toboolean(L, index) != 0);
    }

    bool luax_checkboolean(lua_State* L, int index)
    {
        luaL_checktype(L, index, LUA_TBOOLEAN);
        return luax_toboolean(L, index);
    }

    void luax_pushboolean(lua_State* L, bool boolean)
    {
        lua_pushboolean(L, boolean ? 1 : 0);
    }

    bool luax_optboolean(lua_State* L, int index, bool default_value)
    {
        if (lua_isboolean(L, index) == 1)
            return (lua_toboolean(L, index) == 1 ? true : false);

        return default_value;
    }

    std::string luax_tostring(lua_State* L, int index)
    {
        size_t length;
        const char* string = lua_tolstring(L, index, &length);

        return std::string(string, length);
    }

    std::string luax_checkstring(lua_State* L, int index)
    {
        size_t length;
        const char* string = luaL_checklstring(L, index, &length);

        return std::string(string, length);
    }

    void luax_pushstring(lua_State* L, std::string_view string)
    {
        lua_pushlstring(L, string.data(), string.size());
    }

    void luax_pushpointerasstring(lua_State* L, const void* pointer)
    {
        char string[sizeof(void*)] {};
        std::memcpy(string, &pointer, sizeof(void*));

        lua_pushlstring(L, string, sizeof(void*));
    }

    int luax_ioerror(lua_State* L, const char* format, ...)
    {
        std::va_list args;
        va_start(args, format);

        lua_pushnil(L);
        lua_pushvfstring(L, format, args);

        va_end(args);

        return 2;
    }

    size_t luax_objlen(lua_State* L, int index)
    {
#if LUA_VERSION_NUM == 501
        return lua_objlen(L, index);
#else
        return lua_rawlen(L, index);
#endif
    }

    int luax_table_insert(lua_State* L, int tableIndex, int vIndex, int position)
    {
        if (tableIndex < 0)
            tableIndex = lua_gettop(L) + 1 + tableIndex;

        if (vIndex < 0)
            vIndex = lua_gettop(L) + 1 + vIndex;

        if (position == -1)
        {
            lua_pushvalue(L, vIndex);
            lua_rawseti(L, tableIndex, (int)luax_objlen(L, tableIndex) + 1);

            return 0;
        }
        else if (position < 0)
            position = (int)luax_objlen(L, tableIndex) + 1 + position;

        for (int i = (int)luax_objlen(L, tableIndex) + 1; i > position; i--)
        {
            lua_rawgeti(L, tableIndex, i - 1);
            lua_rawseti(L, tableIndex, i);
        }

        lua_pushvalue(L, vIndex);
        lua_rawseti(L, tableIndex, position);

        return 0;
    }

    int luax_require(lua_State* L, const char* name)
    {
        lua_getglobal(L, "require");
        lua_pushstring(L, name);
        lua_call(L, 1, 1);

        return 1;
    }

    bool luax_isarrayoftables(lua_State* L, int index)
    {
        if (!lua_istable(L, index))
            return false;

        lua_rawgeti(L, index, 1);
        bool tableOfTables = lua_istable(L, -1);

        lua_pop(L, 1);

        return tableOfTables;
    }

    // #endregion

    // #region Registry

    int luax_register_module(lua_State* L, const WrappedModule& module)
    {
        module.type->initialize();

        luax_insistregistry(L, REGISTRY_MODULES);
        Proxy* proxy = (Proxy*)lua_newuserdata(L, sizeof(Proxy));

        proxy->object = module.instance;
        proxy->type   = module.type;

        luaL_newmetatable(L, module.instance->getName());
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, w__gc);
        lua_setfield(L, -2, "__gc");

        lua_setmetatable(L, -2);
        lua_setfield(L, -2, module.name);
        lua_pop(L, 1);

        luax_insistglobal(L, "love");

        lua_newtable(L);

        if (!module.functions.empty())
            luax_register_type_inner(L, module.functions);

        if (!module.types.empty())
            luax_register_types(L, module.types);

        lua_pushvalue(L, -1);
        lua_setfield(L, -3, module.name);
        lua_remove(L, -2);

        return 1;
    }

    void luax_register_type_init(lua_State* L, Type* type)
    {
        type->initialize();

        luax_getregistry(L, REGISTRY_OBJECTS);

        if (!lua_istable(L, -1))
        {
            lua_newtable(L);
            lua_replace(L, -2);

            lua_newtable(L);

            lua_pushliteral(L, "v");
            lua_setfield(L, -2, "__mode");

            lua_setmetatable(L, -2);

            lua_setfield(L, LUA_REGISTRYINDEX, OBJECTS_REGISTRY_KEY);
        }
        else
            lua_pop(L, 1);

        luaL_newmetatable(L, type->getName());

        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, w__gc);
        lua_setfield(L, -2, "__gc");

        lua_pushcfunction(L, w__eq);
        lua_setfield(L, -2, "__eq");

        lua_pushstring(L, type->getName());
        lua_pushcclosure(L, w__tostring, 1);
        lua_setfield(L, -2, "__tostring");

        lua_pushstring(L, type->getName());
        lua_pushcclosure(L, w_type, 1);
        lua_setfield(L, -2, "type");

        lua_pushcfunction(L, w_typeOf);
        lua_setfield(L, -2, "typeOf");

        lua_pushcfunction(L, w_release);
        lua_setfield(L, -2, "release");
    }

    void luax_register_type_inner(lua_State* L, std::span<const luaL_Reg> functions)
    {
        for (const auto& registry : functions)
        {
            lua_pushcfunction(L, registry.func);
            lua_setfield(L, -2, registry.name);
        }
    }

    void luax_register_types(lua_State* L, std::span<const lua_CFunction> types)
    {
        for (const auto& registry : types)
            registry(L);
    }

    // #endregion
} // namespace love
