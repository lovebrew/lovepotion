#pragma once

#include <common/type.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/bidirectionalmap/smallvector.hpp>

extern "C"
{
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>

#include <l53strlib.h>
#include <lutf8lib.h>

    extern int luaopen_bit(lua_State*);
}

#include <numeric>
#include <ranges>
#include <set>
#include <span>
#include <vector>

namespace love
{
    class Object;
    class Module;
    class Variant;

    template<typename T>
    class StrongReference;

    enum Registry
    {
        REGISTRY_OBJECTS,
        REGISTRY_MODULES
    };

    struct Proxy
    {
        Type* type;
        Object* object;
    };

    struct WrappedModule
    {
        const char* name;
        Type* type;

        std::span<const luaL_Reg> functions;
        std::span<const luaL_Reg> extendedFunctions;
        const lua_CFunction* types;

        Module* instance;
    };
} // namespace love

typedef uint64_t objectkey_t;

namespace luax
{
    /* ----- main stuff ----- */

    int Preload(lua_State* L, lua_CFunction function, const char* name);

    int Require(lua_State* L, const char* name);

    int InsistGlobal(lua_State* L, const char* field);

    lua_State* InsistPinnedThread(lua_State* L);

    lua_State* GetPinnedThread(lua_State* L);

    int Insist(lua_State* L, int index, const char* key);

    int InsistLOVE(lua_State* L, const char* key);

    int GetLOVE(lua_State* L, const char* key);

    int RegisterModule(lua_State* L, const love::WrappedModule& moduleName);

    int RegisterSearcher(lua_State* L, lua_CFunction function, int position);

    // int RegisterType(lua_State* L, love::Type* object, ...);

    void RegisterTypeInit(lua_State* L, love::Type* type);

    void RegisterTypeInner(lua_State* L, std::span<const luaL_Reg> values);

    template<typename... T>
    inline int RegisterType(lua_State* L, love::Type* type, T&&... values)
    {
        RegisterTypeInit(L, type);
        (RegisterTypeInner(L, std::forward<T>(values)), ...);

        lua_pop(L, 1);

        return 0;
    }

    int TableInsert(lua_State* L, int index, int vindex, int position);

    int GetLOVEFunction(lua_State* L, const char* mod, const char* fn);

    void SetFunctions(lua_State* L, const luaL_Reg* l);

    void RawNewType(lua_State* L, love::Type& type, love::Object* object);

    objectkey_t ComputeObjectKey(lua_State* L, love::Object* object);

    void PushObjectKey(lua_State* L, objectkey_t key);

    love::Type* Type(lua_State* L, int idx);

    int Resume(lua_State* L, int numArgs);

    /* ----- registry ----- */

    int InsistRegistry(lua_State* L, love::Registry registry);

    int GetRegistry(lua_State* L, love::Registry registry);

    /* ---- object functionality ----- */

    int Equal(lua_State* L);

    bool IsType(lua_State* L, int index, love::Type& type);

    int GarbageCollect(lua_State* L);

    int Release(lua_State* L);

    int ToString(lua_State* L);

    int Type(lua_State* L);

    int TypeOf(lua_State* L);

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

    int TypeError(lua_State* L, int narg, const char* name);

    template<typename T>
    T* CheckType(lua_State* L, int index, const love::Type& type)
    {
        // If not userdata, error out
        if (lua_type(L, index) != LUA_TUSERDATA)
        {
            const char* name = type.GetName();
            luax::TypeError(L, index, name);
        }

        love::Proxy* proxy = (love::Proxy*)lua_touserdata(L, index);

        // Check that it has a type and matches input
        if (proxy->type == nullptr || !proxy->type->IsA(type))
        {
            const char* name = type.GetName();
            luax::TypeError(L, index, name);
        }

        if (proxy->object == nullptr)
            luaL_error(L, "Cannot use object after it has been released.");

        return (T*)proxy->object;
    }

    template<typename T>
    T* CheckType(lua_State* L, int index)
    {
        return CheckType<T>(L, index, T::type);
    }

    void PushType(lua_State* L, love::Type& type, love::Object* object);

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

    int ConvertObject(lua_State* L, int idx, const char* mod, const char* fn);

    int ConvertObject(lua_State* L, const int idxs[], int n, const char* mod, const char* fn);

    int ConvertObject(lua_State* L, const std::vector<int>& idxs, const char* moduleName,
                      const char* function);

    /* helper functions */

    inline void PushString(lua_State* L, std::string_view str)
    {
        lua_pushlstring(L, str.data(), str.size());
    }

    inline bool OptBoolean(lua_State* L, int index, bool boolean)
    {
        if (lua_isboolean(L, index) == 1)
            return (lua_toboolean(L, index) == 1) ? true : false;

        return boolean;
    }

    inline std::string_view OptString(lua_State* L, int index, std::string_view string)
    {
        if (lua_isstring(L, index) == 1)
        {
            size_t length = 0;
            return lua_tolstring(L, index, &length);
        }

        return string;
    }

    inline lua_Number CheckNumberClamped(lua_State* L, int index, double min, double max)
    {
        return std::clamp(luaL_checknumber(L, index), min, max);
    }

    inline lua_Number OptNumberClamped(lua_State* L, int index, double min, double max,
                                       double defaultValue)
    {
        return std::clamp(luaL_optnumber(L, index, defaultValue), min, max);
    }

    inline lua_Number CheckNumberClamped01(lua_State* L, int index)
    {
        return CheckNumberClamped(L, index, 0.0, 1.0);
    }

    inline lua_Number OptNumberClamped01(lua_State* L, int index, double defaultValue)
    {
        return OptNumberClamped(L, index, 0.0, 1.0, defaultValue);
    }

    inline bool ToBoolean(lua_State* L, int index)
    {
        return (lua_toboolean(L, index) != 0);
    }

    inline bool CheckBoolean(lua_State* L, int index)
    {
        luaL_checktype(L, index, LUA_TBOOLEAN);
        return luax::ToBoolean(L, index);
    }

    inline float CheckFloat(lua_State* L, int index)
    {
        return static_cast<float>(luaL_checknumber(L, index));
    }

    inline std::string CheckString(lua_State* L, int index)
    {
        size_t length      = 0;
        const char* string = lua_tolstring(L, index, &length);

        return std::string(string, length);
    }

    inline std::string ToString(lua_State* L, int index)
    {
        size_t length;
        const char* string = lua_tolstring(L, index, &length);

        return std::string(string, length);
    }

    void GetTypeMetaTable(lua_State* L, const love::Type& type);

    void WrapObject(lua_State* L, const char* filedata, size_t length, const char* filename,
                    const love::Type& type);

    inline void PushBoolean(lua_State* L, bool boolean)
    {
        lua_pushboolean(L, boolean ? 1 : 0);
    }

    void PushVariant(lua_State* L, const love::Variant& variant);

    love::Variant CheckVariant(lua_State* L, int index, bool allowUserdata = true,
                               std::set<const void*>* tableSet = nullptr);

    size_t ObjectLength(lua_State* L, int index);

    /* ----- errors ----- */

    int IOError(lua_State* L, const char* format, ...);

    int EnumError(lua_State* L, const char* enumName, const char* value);

    template<typename T>
    void CheckTableFields(lua_State* L, int idx, const char* enumName,
                          bool (*getConstant)(const char*))
    {
        luaL_checktype(L, idx, LUA_TTABLE);

        /* we want to error for invalid / misspelled */
        /* fields in the table. */

        lua_pushnil(L);

        while (lua_next(L, idx))
        {
            if (lua_type(L, -2) != LUA_TSTRING)
                luax::TypeError(L, -2, "string");

            const char* key = luaL_checkstring(L, -2);

            bool value;
            if (!(value = getConstant(key)))
                luax::EnumError(L, enumName, key);

            lua_pop(L, 1);
        }
    }

    bool BoolFlag(lua_State* L, int tableIndex, const char* key, bool defaultValue);

    int IntFlag(lua_State* L, int tableIndex, const char* key, int defaultValue);

    double NumberFlag(lua_State* L, int tableIndex, const char* key, double defaultValue);

    int CheckIntFlag(lua_State* L, int tableIndex, const char* key);

    inline std::string concat(std::string&& first, const std::string_view& second)
    {
        return first.empty() ? std::string(second) : first + ", " + std::string(second);
    }

    template<std::ranges::range Range>
    requires(std::is_convertible_v<std::ranges::range_value_t<Range>, std::string_view>)
    int EnumError(lua_State* L, std::string_view type, const Range& values, std::string_view value)
    {
        std::string enums =
            std::accumulate(std::begin(values), std::end(values), std::string {}, concat);

        const char* enumValue = std::string(value).c_str();
        const char* enumType  = std::string(type).c_str();

        return luaL_error(L, "Invalid %s '%s', expected one of: %s", enumType, enumValue,
                          enums.c_str());
    }

    // clang-format off
    template<typename K, typename V, std::size_t S, std::equivalence_relation<K, K> KC, std::equivalence_relation<V, V> VC>
    int EnumError(lua_State* L, std::string_view enumName, const BidirectionalMap<K, V, S, KC, VC>& map, std::string_view value)
    {
        return EnumError(L, enumName, map.GetNames(), value);
    }
    // clang-format on

    int Traceback(lua_State* L);

    template<typename T>
    int CatchException(lua_State* L, const T& func)
    {
        bool shouldError = false;

        try
        {
            func();
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

    template<int count>
    int AreArgsNil(lua_State* L, int start)
    {
        for (size_t index = 0; index < count; index++)
        {
            if (!lua_isnil(L, start + index))
                return false;
        }

        return true;
    }

    int AssertNilError(lua_State* L, int idx);
} // namespace luax
