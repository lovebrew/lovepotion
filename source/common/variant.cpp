#include "common/variant.h"

using namespace love;

Proxy* Variant::TryExtractProxy(lua_State* L, size_t index)
{
    Proxy* proxy = (Proxy*)lua_touserdata(L, index);

    if (!proxy || !proxy->type)
        return nullptr;

    if (dynamic_cast<Object*>(proxy->object) != nullptr)
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
    else if (self == Type::TABLE)
        this->GetValue<Type::TABLE>()->Release();
}

Variant::Variant(love::Type* loveType, Object* object)
{
    Proxy proxy;

    proxy.type   = loveType;
    proxy.object = object;

    if (proxy.object != nullptr)
        proxy.object->Retain();

    this->variant = proxy;
}

Variant::Variant(const char* string, size_t length)
{
    if (length <= MAX_SMALL_STRING_LENGTH)
    {
        SmallString str = SmallString();
        memcpy(str.string, string, length);
        str.length = length;

        this->variant = str;
    }
    else
        this->variant = new SharedString(string, length);
}

Variant::Variant(const std::string& string) : Variant(string.c_str(), string.length())
{}

Variant& Variant::operator=(const Variant& v)
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
    else if (other == Type::TABLE)
        v.GetValue<Type::TABLE>()->Retain();

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
    else if (self == Type::TABLE)
        this->GetValue<Type::TABLE>()->Release();

    variant = v.variant;

    return *this;
}

Variant::Variant(std::vector<std::pair<Variant, Variant>>* table)
{
    this->variant = new SharedTable(table);
}

Variant::Variant(const Variant& other) : variant(other.variant)
{
    if (this->GetType() == Type::STRING)
        this->GetValue<Type::STRING>()->Retain();
    else if (this->GetType() == Type::LOVE_OBJECT &&
             this->GetValue<Type::LOVE_OBJECT>().object != nullptr)
        this->GetValue<Type::LOVE_OBJECT>().object->Retain();
    else if (this->GetType() == Type::TABLE)
        this->GetValue<Type::TABLE>()->Retain();
}

Variant::Variant(Variant&& other) : variant(std::move(other.variant))
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
    else if (type == Type::TABLE)
        return "table";

    return "unknown";
}

Variant::Type Variant::GetType() const
{
    Type retval = (Type)variant.index();

    return retval;
}

Variant Variant::FromLua(lua_State* L, int n, std::set<const void*>* tableSet)
{
    Proxy* proxy = nullptr;
    const char* string;
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
        case LUA_TTABLE:
        {
            bool success = true;
            std::set<const void*> topTableSet;

            std::vector<std::pair<Variant, Variant>>* table =
                new std::vector<std::pair<Variant, Variant>>();

            // We can use a pointer to a stack-allocated variable because it's
            // never used after the stack-allocated variable is destroyed.
            if (tableSet == nullptr)
                tableSet = &topTableSet;

            // Check it's not already serialized
            const void* tablePointer = lua_topointer(L, n);

            {
                auto result = tableSet->insert(tablePointer);
                if (!result.second) // Insertion failure
                    throw love::Exception("Cycle detected in table.");
            }

            size_t length = lua_objlen(L, -1);
            if (length > 0)
                table->reserve(length);

            lua_pushnil(L);

            while (lua_next(L, n))
            {
                table->emplace_back(FromLua(L, -2, tableSet), FromLua(L, -1, tableSet));
                lua_pop(L, 1);

                const auto& p = table->back();

                if (p.first.GetType() == Type::UNKNOWN || p.second.GetType() == Type::UNKNOWN)
                {
                    success = false;
                    break;
                }
            }

            // remove the table from the set
            tableSet->erase(tablePointer);

            if (success)
                return Variant(table);
            else
                delete table;
        }
        default:
            break;
    }

    return Variant(std::monostate());
}

void Variant::ToLua(lua_State* L) const
{
    SharedString* str;
    SmallString smallStr;
    Proxy proxy;
    bool boolean;
    float number;
    void* lightUserdata;

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
        case Type::TABLE:
        {
            auto table  = GetValue<Type::TABLE>()->table;
            size_t size = table->size();

            lua_createtable(L, 0, size);

            for (size_t index = 0; index < size; ++index)
            {
                std::pair<Variant, Variant>& keyValue = (*table)[index];

                keyValue.first.ToLua(L);
                keyValue.second.ToLua(L);

                lua_settable(L, -3);
            }

            break;
        }
        case Type::NIL:
        default:
            lua_pushnil(L);
            break;
    }
}
