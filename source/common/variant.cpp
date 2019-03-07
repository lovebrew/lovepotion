#include "common/runtime_common.h"

Variant::Variant()
{
    this->tag = VARIANTS::NIL;
}

Variant::~Variant()
{
    if (this->data_string)
        free(this->data_string);
}

Variant::Variant(double number)
{
    this->tag = VARIANTS::NUMBER;
    this->data_number = number;

    printf("Variant: %.1f\n", number);
}

Variant::Variant(void * data)
{
    this->tag = VARIANTS::USERDATA;
    this->data_pointer = data;
}

Variant::Variant(const std::string & sstring)
{
    this->tag = VARIANTS::STRING;
    this->data_string = strdup(sstring.c_str());

    printf("Variant: %s\n", sstring.c_str());
}

Variant::Variant(bool boolean)
{
    this->tag = VARIANTS::BOOLEAN;
    this->data_boolean = boolean;
}

Variant Variant::FromLua(lua_State * L, int index)
{
    if (index < 0)
        index += lua_gettop(L) + 1;

    int type = lua_type(L, index);

    LOG("Type %d at Index %d\n", type, index);

    switch (type)
    {
        case LUA_TNUMBER:
            return Variant(lua_tonumber(L, index));
        case LUA_TSTRING:
            return Variant(lua_tostring(L, index));
        default:
            return Variant();
    }

    return Variant();
}

void Variant::ToLua(lua_State * L)
{
    switch (this->tag)
    {
        case VARIANTS::NUMBER:
            lua_pushnumber(L, this->data_number);
            break;
        case VARIANTS::STRING:
            lua_pushstring(L, this->data_string);
            break;
        case VARIANTS::BOOLEAN:
            lua_pushboolean(L, this->data_boolean);
            break;
        default:
            lua_pushnil(L);
            break;
    }
}