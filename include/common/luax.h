/*
** common/aux.h
** @brief   : Auxillary Lua functions
*/

#pragma once

#define MAX_LUAOBJ_KEY 0x20000000000000ULL

#include "common/type.h"

extern "C"
{
#include "lua/lauxlib.h"
#include "lua/lua.h"
#include "lua/lualib.h"

#include "lua53/l53strlib.h"
#include "lua53/lutf8lib.h"
}

#include <algorithm>
#include <exception>
#include <memory>
#include <string.h>

#include "common/reference.h"

namespace love
{
    class Object;
    class Module;

    template<typename T>
    class StrongReference;

    enum Registry
    {
        REGISTRY_OBJECTS,
        REGISTRY_MODULES
    };

    struct Proxy
    {
        love::Type* type; //< Holds type information (see types.h).
        Object* object;   //<   Pointer to the actual object.
    };

    struct WrappedModule
    {
        const char* name;
        love::Type* type;

        const luaL_Reg* functions;
        const lua_CFunction* types;

        love::Module* instance;
    };
} // namespace love

namespace Luax
{
    int Preload(lua_State* L, lua_CFunction func, const char* name);

    int TableInsert(lua_State* L, int index, int vindex, int position);

    inline bool OptBoolean(lua_State* L, int index, bool boolean)
    {
        if (lua_isboolean(L, index) == 1)
            return (lua_toboolean(L, index) == 1) ? true : false;

        return boolean;
    }

    inline std::string ToString(lua_State* L, int index)
    {
        size_t length;
        const char* string = lua_tolstring(L, index, &length);

        return std::string(string, length);
    }

    inline std::string CheckString(lua_State* L, int index)
    {
        size_t length;
        const char* string = luaL_checklstring(L, index, &length);

        return std::string(string, length);
    }

    inline void PushString(lua_State* L, const std::string& str)
    {
        lua_pushlstring(L, str.data(), str.size());
    }

    int RegisterSearcher(lua_State* L, lua_CFunction function, int position);

    love::Reference* RefIf(lua_State* L, int type);

    int Resume(lua_State* L, int nargs);

    int DoBuffer(lua_State* L, const char* buffer, size_t size, const char* name);

    int InsistGlobal(lua_State* L, const char* field);

    int Insist(lua_State* L, int index, const char* key);

    int InsistLove(lua_State* L, const char* key);

    int GetLove(lua_State* L, const char* key);

    lua_State* InsistPinnedThread(lua_State* L);

    lua_State* GetPinnedThread(lua_State* L);

    /* REGISTRY */

    int GetRegistry(lua_State* L, love::Registry registry);

    int InsistRegistry(lua_State* L, love::Registry registry);

    int Require(lua_State* L, const char* name);

    int ConvertObject(lua_State* L, int idx, const char* mod, const char* fn);

    int ConvertObject(lua_State* L, const int idxs[], int n, const char* mod, const char* fn);

    int ConvertObject(lua_State* L, const std::vector<int>& idxs, const char* moduleName,
                      const char* function);

    int AssertNilError(lua_State* L, int idx);

    int GetLOVEFunction(lua_State* L, const char* mod, const char* fn);

    void SetFunctions(lua_State* L, const luaL_Reg* l);

    int RegisterModule(lua_State* L, const love::WrappedModule& moduleName);

    int RegisterType(lua_State* L, love::Type* object, ...);

    int GarbageCollect(lua_State* L);

    int ToString(lua_State* L);

    int Equal(lua_State* L);

    love::Type* Type(lua_State* L, int idx);

    inline lua_Number CheckNumberClamped01(lua_State* L, int index)
    {
        return std::min(std::max(luaL_checknumber(L, index), 0.0), 1.0);
    }

    inline lua_Number OptNumberClamped01(lua_State* L, int index, double def)
    {
        return std::min(std::max(luaL_optnumber(L, index, def), 0.0), 1.0);
    }

    void GetTypeMetaTable(lua_State* L, const love::Type& type);

    void RunWrapper(lua_State* L, const char* filedata, size_t length, const char* filename,
                    const love::Type& type);

    int Type(lua_State* L);

    int TypeOf(lua_State* L);

    int Release(lua_State* L);

    bool IsType(lua_State* L, int index, love::Type& type);

    void RawNewType(lua_State* L, love::Type& type, love::Object* object);

    lua_Number ComputeObjectKey(lua_State* L, love::Object* object);

    int IOError(lua_State* L, const char* format, ...);

    int EnumError(lua_State* L, const char* enumName, const char* value);

    int EnumError(lua_State* L, const char* enumName, const std::vector<const char*>& values,
                  const char* value);

    void PushType(lua_State* L, love::Type& type, love::Object* object);

    /* flags for tables */

    bool BoolFlag(lua_State* L, int table_index, const char* key, bool defaultValue);

    int IntFlag(lua_State* L, int table_index, const char* key, int defaultValue);

    double NumberFlag(lua_State* L, int table_index, const char* key, double defaultValue);

    /* end flags for tables */

    bool ToBoolean(lua_State* L, int index);

    bool CheckBoolean(lua_State* L, int index);

    void PushBoolean(lua_State* L, bool boolean);

    template<typename T>
    void PushType(lua_State* L, T* object)
    {
        PushType(L, T::type, object);
    }

    template<typename T>
    void PushType(lua_State* L, love::StrongReference<T>& object)
    {
        PushType(L, T::type, object);
    }

    int TypeErrror(lua_State* L, int narg, const char* name);

    template<int min, int max = -1>
    int AssertArgc(lua_State* L)
    {
        int argc = lua_gettop(L);

        if (argc < min)
            return luaL_error(L, "Incorrect number of arguments. Expected at least %d", min);

        if (max != -1 && argc > max)
            return luaL_error(L, "Incorrect number of arguments. Got [%d], expected [%d-%d]", argc,
                              min, max);
        return 0;
    }

    inline bool IsCTR()
    {
        if (strncmp(__CONSOLE__, "3DS", 3))
            return true;

        return false;
    }

    template<typename T>
    T* ToType(lua_State* L, int index, const love::Type& /*type*/)
    {
        T* object = (T*)(((love::Proxy*)lua_touserdata(L, index))->object);

        if (object == nullptr)
            luaL_error(L, "Cannot use object after it has been released.");

        return object;
    }

    template<typename T>
    T* ToType(lua_State* L, int index)
    {
        return ToType<T>(L, index, T::type);
    }

    template<typename T>
    T* CheckType(lua_State* L, int index, const love::Type& type)
    {
        // If not userdata, error out
        if (lua_type(L, index) != LUA_TUSERDATA)
        {
            const char* name = type.GetName();
            Luax::TypeErrror(L, index, name);
        }

        love::Proxy* proxy = (love::Proxy*)lua_touserdata(L, index);

        // Check that it has a type and matches input
        if (proxy->type == nullptr || !proxy->type->IsA(type))
        {
            const char* name = type.GetName();
            Luax::TypeErrror(L, index, name);
        }

        if (proxy->object == nullptr)
            luaL_error(L, "Cannot use object after it has been released.");

        return (T*)proxy->object;
    }

    template<size_t count>
    bool ArgcIsNil(lua_State* L)
    {
        for (size_t i = 1; i <= count; i++)
        {
            if (!lua_isnil(L, i))
                return false;
        }

        return true;
    }

    template<typename T>
    T* CheckType(lua_State* L, int index)
    {
        return CheckType<T>(L, index, T::type);
    }

    int Traceback(lua_State* L);

    template<typename T>
    int CatchException(lua_State* L, const T& func)
    {
        bool should_error = false;

        try
        {
            func();
        }
        catch (const std::exception& e)
        {
            should_error = true;
            lua_pushstring(L, e.what());
        }

        if (should_error)
            return luaL_error(L, "%s", lua_tostring(L, -1));

        return 0;
    }

    template<typename T>
    void CheckTableFields(lua_State* L, int idx, const char* enumName,
                          bool (*getConstant)(const char*, T&))
    {
        luaL_checktype(L, idx, LUA_TTABLE);

        /*
        ** We want to error for invalid / misspelled
        ** fields in the table.
        */
        lua_pushnil(L);
        while (lua_next(L, idx))
        {
            if (lua_type(L, -2) != LUA_TSTRING)
                Luax::TypeErrror(L, -2, "string");

            const char* key = luaL_checkstring(L, -2);
            T constantvalue;

            if (!getConstant(key, constantvalue))
                Luax::EnumError(L, enumName, key);

            lua_pop(L, 1);
        }
    }

    template<typename T, typename F>
    int CatchException(lua_State* L, const T& func, const F& finallyfunc)
    {
        bool should_error = false;

        try
        {
            func();
        }
        catch (const std::exception& e)
        {
            should_error = true;
            lua_pushstring(L, e.what());
        }

        finallyfunc(should_error);

        if (should_error)
            return luaL_error(L, "%s", lua_tostring(L, -1));

        return 0;
    }
}; // namespace Luax
