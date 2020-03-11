/*
** common/aux.h
** @brief   : Auxillary Lua functions
*/

#pragma once

enum Registry
{
    OBJECTS = 0,
    MODULES,
    UNKNOWN
};

#define MAX_LUAOBJ_KEY 0x20000000000000ULL

#include "objects/object.h"
#include "common/type.h"

namespace Luax
{
    int DoBuffer(lua_State * L, const char * buffer, size_t size, const char * name);

    int InsistGlobal(lua_State * L, const char * field);

    int Insist(lua_State * L, int index, const char * key);

    int InsistLove(lua_State * L, const char * key);

    int GetLove(lua_State * L, const char * key);

    lua_State * InsistPinnedThread(lua_State * L);

    int Require(lua_State * L, const char * name);

    int ConvertObject(lua_State *L, int idx, const char  *mod, const char * fn);

    int ConvertObject(lua_State *L, const int idxs[], int n, const char *mod, const char *fn);

    int ConvertObject(lua_State *L, const std::vector<int>& idxs, const char *module, const char *function);

    int AssertNilError(lua_State *L, int idx);

    int GetLOVEFunction(lua_State *L, const char * mod, const char * fn);

    void SetFunctions(lua_State * L, const luaL_reg * l);

    int RegisterModule(lua_State * L, const love::WrappedModule & module);

    int RegisterType(lua_State * L, love::Type * object, ...);

    int GarbageCollect(lua_State * L);

    int ToString(lua_State * L);

    int Equal(lua_State * L);

    love::Type * Type(lua_State *L, int idx);

    inline lua_Number CheckNumberClamped01(lua_State *L, int index)
    {
        return std::min(std::max(luaL_checknumber(L, index), 0.0), 1.0);
    }

    inline lua_Number OptNumberClamped01(lua_State *L, int index, double def)
    {
        return std::min(std::max(luaL_optnumber(L, index, def), 0.0), 1.0);
    }

    void GetTypeMetaTable(lua_State * L, const love::Type & type);

    void RunWrapper(lua_State * L, const char * filedata, size_t length, const char * filename, const love::Type & type);

    int Type(lua_State * L);

    int TypeOf(lua_State * L);

    int Release(lua_State * L);

    bool IsType(lua_State * L, int index, love::Type & type);

    void RawNewType(lua_State * L, love::Type & type, love::Object * object);

    lua_Number ComputerObjectKey(lua_State * L, love::Object * object);

    int IOError(lua_State * L, const char * format, ...);

    int EnumError(lua_State * L, const char * enumName, const char * value);

    int EnumError(lua_State * L, const char * enumName, const std::vector<std::string> & values, const char * value);

    void PushType(lua_State * L, love::Type & type, love::Object * object);

    template <typename T>
    void PushType(lua_State * L, T * object)
    {
        PushType(L, T::type, object);
    }

    template <typename T>
    void PushType(lua_State * L, love::StrongReference<T> & object)
    {
        PushType(L, T::type, object);
    }

    int TypeErrror(lua_State * L, int narg, const char * name);

    template <typename T>
    T * ToType(lua_State * L, int index, const love::Type & /*type*/)
    {
        T * object = (T *)(((love::Proxy *)lua_touserdata(L, index))->object);

        if (object == nullptr)
            luaL_error(L, "Cannot use object after it has been released.");

        return object;
    }

    template <typename T>
    T * ToType(lua_State * L, int index)
    {
        return ToType<T>(L, index, T::type);
    }

    /*
    ** @func CheckType<T>
    ** Checks if the value on the Lua stack @index is of @type
    ** See: https://github.com/love2d/love/blob/master/src/common/runtime.h#L528
    */
    template <typename T>
    T * CheckType(lua_State * L, int index, const love::Type & type)
    {
        // If not userdata, error out
        if (lua_type(L, index) != LUA_TUSERDATA)
        {
            const char * name = type.GetName();
            Luax::TypeErrror(L, index, name);
        }

        love::Proxy * proxy = (love::Proxy *)lua_touserdata(L, index);

        // Check that it has a type and matches input
        if (proxy->type == nullptr || !proxy->type->IsA(type))
        {
            const char * name = type.GetName();
            Luax::TypeErrror(L, index, name);
        }

        if (proxy->object == nullptr)
            luaL_error(L, "Cannot use object after it has been released.");

        return (T *)proxy->object;
    }

    template <typename T>
    T * CheckType(lua_State * L, int index)
    {
        return CheckType<T>(L, index, T::type);
    }

    int Traceback(lua_State * L);

    template <typename T>
    int CatchException(lua_State * L, const T & func)
    {
        bool should_error = false;

        try
        {
            func();
        }
        catch (const std::exception & e)
        {
            should_error = true;
            lua_pushstring(L, e.what());
        }

        if (should_error)
            return luaL_error(L, "%s", lua_tostring(L, -1));

        return 0;
    }

    template <typename T, typename F>
    int CatchException(lua_State * L, const T & func, const F & finallyfunc)
    {
        bool should_error = false;

        try
        {
            func();
        }
        catch (const std::exception & e)
        {
            should_error = true;
            lua_pushstring(L, e.what());
        }

        finallyfunc(should_error);

        if (should_error)
            return luaL_error(L, "%s", lua_tostring(L, -1));

        return 0;
    }
};
