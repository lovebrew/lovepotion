#include "common/runtime.h"

using namespace love;

Proxy * Variant::TryExtractProxy(lua_State * L, size_t index)
{
    Proxy * proxy = (Proxy *)lua_touserdata(L, index);

    if (!proxy || !proxy->type)
        return nullptr;

    if (dynamic_cast<Object *>(proxy->object) != nullptr)
        return proxy;

    return nullptr;
}

Variant::~Variant()
{
    Type self = this->GetType();
    Proxy selfProxy;

    if (self == Type::LOVE_OBJECT)
    {
        selfProxy = GetValue<Type::LOVE_OBJECT>();

        if (selfProxy.object != nullptr)
            selfProxy.object->Release();
    }
}

Variant::Variant(love::Type * loveType, Object * object)
{
    Proxy proxy;

    proxy.type = loveType;
    proxy.object = object;

    if (proxy.object != nullptr)
        proxy.object->Retain();

    this->variant = proxy;
}

Variant & Variant::operator=(const Variant & v)
{
    Type other = v.GetType();

    Proxy otherProxy;
    if (other == Type::LOVE_OBJECT)
    {
        otherProxy = v.GetValue<Type::LOVE_OBJECT>();
        otherProxy.object->Retain();
    }

    Type self = this->GetType();
    Proxy selfProxy;

    if (self == Type::LOVE_OBJECT)
    {
        selfProxy = GetValue<Type::LOVE_OBJECT>();
        selfProxy.object->Release();
    }

    variant = v.variant;

    return *this;
}

std::string Variant::GetTypeString() const
{
    Type type = GetType();

    if (type == Type::BOOLEAN)
        return "boolean";
    else if (type == Type::NIL)
        return "nil";
    else if (type == Type::NUMBER)
        return "number";
    else if (type == Type::STRING)
        return "string";
    else if (type == Type::LOVE_OBJECT)
        return "object";
    else if (type == Type::LUSERDATA)
        return "light userdata";

    return "unknown";
}

Variant::Type Variant::GetType() const
{
    Type retval = (Type)variant.index();

    return retval;
}

Variant Variant::FromLua(lua_State * L, int n)
{
    std::string string;
    bool boolean;
    float number;
    Proxy * proxy = nullptr;

    // Defaults to std::monostate, aka Type::UNKNOWN
    Variant retval;

    if (n < 0)
        n += lua_gettop(L) + 1;

    switch (lua_type(L, n))
    {
        case LUA_TSTRING:
            string = lua_tostring(L, n);
            retval = Variant(string);
            return retval;
        case LUA_TNIL:
            retval = Variant(Nil());
            return retval;
        case LUA_TBOOLEAN:
            boolean = lua_toboolean(L, n);
            retval = Variant(boolean);
            return retval;
        case LUA_TNUMBER:
            number = lua_tonumber(L, n);
            retval = Variant(number);
            return retval;
        case LUA_TUSERDATA:
            proxy = TryExtractProxy(L, n);
            if (proxy != nullptr)
                return Variant(proxy->type, proxy->object);
            else
            {
                Luax::TypeErrror(L, n, "love type");
                return Variant(Nil());
            }
        default:
            break;
    }

    return Variant(std::monostate());
}

void Variant::ToLua(lua_State * L) const
{
    std::string string;
    Proxy proxy;
    bool boolean;
    float number;
    void * lightUserdata;

    switch (this->GetType())
    {
        case Type::STRING:
            string = GetValue<Type::STRING>();
            lua_pushlstring(L, string.data(), string.size());
            break;
        case Type::LOVE_OBJECT:
            proxy = GetValue<Type::LOVE_OBJECT>();
            Luax::PushType(L, *proxy.type, proxy.object);
            break;
        case Type::BOOLEAN:
            boolean = GetValue<Type::BOOLEAN>();
            lua_pushboolean(L, boolean);
            break;
        case Type::NUMBER:
            number = GetValue<Type::NUMBER>();
            lua_pushnumber(L, number);
            break;
        case Type::LUSERDATA:
            lightUserdata = GetValue<Type::LUSERDATA>();
            lua_pushlightuserdata(L, lightUserdata);
            break;
        case Type::NIL:
        default:
            lua_pushnil(L);
            break;
    }
}
