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
    else if (self == Type::STRING)
        this->GetValue<Type::STRING>()->Release();
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

Variant::Variant(const char * string, size_t length)
{
    if (length <= MAX_SMALL_STRING_LENGTH)
    {
        SmallString str = SmallString();
        memcpy(str.string, string, length);
        str.length = length;

        this->variant = str;
    }
    else
    {
        SharedString * str = new SharedString(string, length);
        this->variant = str;
    }
}

Variant::Variant(const std::string & string) : Variant(string.c_str(), string.length())
{}

Variant & Variant::operator=(const Variant & v)
{
    Type other = v.GetType();

    Proxy otherProxy;
    if (other == Type::LOVE_OBJECT)
    {
        otherProxy = v.GetValue<Type::LOVE_OBJECT>();

        if (otherProxy.object != nullptr)
            otherProxy.object->Retain();
    }
    else if (other == Type::STRING)
        v.GetValue<Type::STRING>()->Retain();

    Type self = this->GetType();
    Proxy selfProxy;

    if (self == Type::LOVE_OBJECT)
    {
        selfProxy = GetValue<Type::LOVE_OBJECT>();

        if (selfProxy.object != nullptr)
            selfProxy.object->Release();
    }
    else if (self == Type::STRING)
        this->GetValue<Type::STRING>()->Release();

    variant = v.variant;

    return *this;
}

Variant::Variant(const Variant & other) : variant(other.variant)
{
    if (this->GetType() == Type::STRING)
        this->GetValue<Type::STRING>()->Retain();
    else if (this->GetType() == Type::LOVE_OBJECT && this->GetValue<Type::LOVE_OBJECT>().object != nullptr)
        this->GetValue<Type::LOVE_OBJECT>().object->Retain();
}

Variant::Variant(Variant && other) : variant(std::move(other.variant))
{}

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
    Proxy * proxy = nullptr;
    const char * string;
    size_t length;

    if (n < 0)
        n += lua_gettop(L) + 1;

    switch (lua_type(L, n))
    {
        case LUA_TSTRING:
            string = lua_tolstring(L, n, &length);
            return Variant(string, length);
        case LUA_TNIL:
            return Variant(Nil());
        case LUA_TBOOLEAN:
            return Variant((bool)lua_toboolean(L, n));
        case LUA_TNUMBER:
            return Variant((float)lua_tonumber(L, n));
        case LUA_TLIGHTUSERDATA:
            return Variant(lua_touserdata(L, n));
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
    SharedString * str;
    SmallString smallStr;
    Proxy proxy;
    bool boolean;
    float number;
    void * lightUserdata;

    switch (this->GetType())
    {
        case Type::SMALLSTRING:
            smallStr = GetValue<Type::SMALLSTRING>();
            lua_pushlstring(L, smallStr.string, smallStr.length);
            break;
        case Type::STRING:
            str = GetValue<Type::STRING>();
            lua_pushlstring(L, str->string, str->length);
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
