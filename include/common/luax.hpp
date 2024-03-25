#pragma once

#include "common/Object.hpp"
#include "common/Variant.hpp"
#include "common/types.hpp"

extern "C"
{
#define LUA_COMPAT_ALL
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <l53strlib.h>
#include <lutf8lib.h>

    extern int luaopen_bit(lua_State*);
}

#include <algorithm>
#include <exception>
#include <set>
#include <span>

#include <cstdarg>

namespace love
{
    class Module;
    class Reference;

    template<typename T>
    class StrongRef;

    enum Registry
    {
        REGISTRY_MODULES,
        REGISTRY_OBJECTS
    };

    struct WrappedModule
    {
        Module* instance;
        const char* name;
        Type* type;
        std::span<const luaL_Reg> functions;
        std::span<const lua_CFunction> types;
    };

    static constexpr uint64_t LUAX_MAX_OBJECT_KEY = 0x20000000000000ULL;

    static constexpr const char* MAIN_THREAD_KEY      = "_love_mainthread";
    static constexpr const char* OBJECTS_REGISTRY_KEY = "_loveobjects";
    static constexpr const char* MODULES_REGISTRY_KEY = "_modules";

    using ObjectKey = uint64_t;

    // #region Startup

    int luax_preload(lua_State* L, lua_CFunction function, const char* name);

    lua_State* luax_insistpinnedthread(lua_State* L);

    lua_State* luax_getpinnedthread(lua_State* L);

    int luax_insist(lua_State* L, int index, const char* name);

    int luax_insistglobal(lua_State* L, const char* name);

    int luax_insistlove(lua_State* L, const char* name);

    int luax_getlove(lua_State* L, const char* name);

    int luax_insistregistry(lua_State* L, Registry registry);

    int luax_getregistry(lua_State* L, Registry registry);

    int luax_isfulllightuserdatasupported(lua_State* L);

    int luax_resume(lua_State* L, int argc, int* nres);

    // #endregion

    // #region Helpers

    int luax_require(lua_State* L, const char* name);

    bool luax_isarrayoftables(lua_State* L, int index);

    bool luax_toboolean(lua_State* L, int index);

    bool luax_checkboolean(lua_State* L, int index);

    void luax_pushboolean(lua_State* L, bool boolean);

    bool luax_optboolean(lua_State* L, int index, bool default_value);

    std::string luax_tostring(lua_State* L, int index);

    std::string luax_checkstring(lua_State* L, int index);

    void luax_pushstring(lua_State* L, std::string_view string);

    void luax_pushpointerasstring(lua_State* L, const void* pointer);

    bool luax_istype(lua_State* L, int index, Type& type);

    Type* luax_type(lua_State* L, int index);

    template<typename T>
    T* luax_totype(lua_State* L, int index, const Type& type)
    {
        if (lua_type(L, index) != LUA_TUSERDATA)
            return nullptr;

        Proxy* userdata = (Proxy*)lua_touserdata(L, index);

        if (userdata->type != nullptr && userdata->type->isA(type))
        {
            if (userdata->object == nullptr)
                luaL_error(L, "Cannot use object after it has been released.");

            return (T*)userdata->object;
        }

        return nullptr;
    }

    template<typename T>
    T* luax_totype(lua_State* L, int index)
    {
        return luax_totype<T>(L, index, T::type);
    }

    int luax_typeerror(lua_State* L, int argc, const char* name);

    template<typename T>
    T* luax_checktype(lua_State* L, int index, const Type& type)
    {
        if (lua_type(L, index) != LUA_TUSERDATA)
        {
            const char* name = type.getName();
            luaL_typerror(L, index, name);
        }

        Proxy* userdata = (Proxy*)lua_touserdata(L, index);

        if (userdata->type == nullptr || !userdata->type->isA(type))
        {
            const char* name = type.getName();
            luax_typeerror(L, index, name);
        }

        if (userdata->object == nullptr)
            luaL_error(L, "Cannot use object after it has been released.");

        return (T*)userdata->object;
    }

    template<typename T>
    T* luax_checktype(lua_State* L, int index)
    {
        return luax_checktype<T>(L, index, T::type);
    }

    template<typename T>
    int luax_catchexcept(lua_State* L, const T& function)
    {
        bool shouldError = false;

        try
        {
            function();
        }
        catch (const std::exception& e)
        {
            shouldError = true;
            lua_pushstring(L, e.what());
        }

        if (shouldError)
            return luaL_error(L, "%s", lua_tostring(L, -1));

        return 0;
    }

    template<typename T, typename F>
    int luax_catchexcept(lua_State* L, const T& function, const F& finally)
    {
        bool shouldError = false;

        try
        {
            function();
        }
        catch (const std::exception& e)
        {
            shouldError = true;
            lua_pushstring(L, e.what());
        }

        finally(shouldError);

        if (shouldError)
            return luaL_error(L, "%s", lua_tostring(L, -1));

        return 0;
    }

    int luax_ioerror(lua_State* L, const char* format, ...);

    int luax_register_searcher(lua_State* L, lua_CFunction function, int index);

    size_t luax_objlen(lua_State* L, int index);

    int luax_table_insert(lua_State* L, int tableIndex, int vIndex, int position);

    void luax_pushvariant(lua_State* L, const Variant& v);

    Variant luax_checkvariant(lua_State* L, int index, bool allowuserdata = true,
                              std::set<const void*>* tableSet = nullptr);

    int luax_convobj(lua_State* L, int index, const char* module, const char* function);

    int luax_convobj(lua_State* L, const int indices[], int argc, const char* module,
                     const char* function);

    int luax_convobj(lua_State* L, const std::vector<int>& indices, const char* module,
                     const char* function);

    int luax_getfunction(lua_State* L, const char* module, const char* name);

    // #endregion

    // #region Registry

    int luax_register_module(lua_State* L, const WrappedModule& module);

    void luax_register_type_init(lua_State* L, Type* type);

    void luax_register_type_inner(lua_State* L, std::span<const luaL_Reg> values);

    template<typename... T>
    inline int luax_register_type(lua_State* L, Type* type, T&&... values)
    {
        luax_register_type_init(L, type);
        (luax_register_type_inner(L, std::forward<T>(values)), ...);

        lua_pop(L, 1);

        return 0;
    }

    void luax_register_types(lua_State* L, std::span<const lua_CFunction> types);

    void luax_rawnewtype(lua_State* L, Type& type, Object* object);

    void luax_pushtype(lua_State* L, Type& type, Object* object);

    template<typename T>
    void luax_pushtype(lua_State* L, T* object)
    {
        luax_pushtype(L, T::type, object);
    }

    template<typename T>
    void luax_pushtype(lua_State* L, StrongRef<T>& object)
    {
        luax_pushtype(L, T::type, object);
    }

    Proxy* luax_tryextractproxy(lua_State* L, int index);

    template<typename T>
    int luax_enumerror(lua_State* L, const char* name, const T& map, std::string_view value)
    {
        std::string expected = map.expected(name, value);
        return luaL_error(L, "%s", expected.c_str());
    }
    // #endregion

    // #region Other

    // #endregion

    // #region Debug

    int luax_traceback(lua_State* L);

    // #endregion
} // namespace love
