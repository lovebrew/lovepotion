#include "common/runtime.h"

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
}

Variant::Variant(bool boolean)
{
	this->tag = VARIANTS::BOOLEAN;
	this->data_boolean = boolean;
}

void Variant::ToLua(lua_State * L)
{
	/*
	** TODO
	** Make Lua actually push this stuff
	** Right now it's dumb printf's :p
	*/
	switch(this->tag)
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