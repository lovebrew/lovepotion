#include "common/runtime.h"
#include <stdarg.h>

Exception::Exception(const string & format, ...)
{
	va_list args;

	va_start(args, format);

	vsprintf((char *)this->message.data(), format.c_str(), args);

	va_end(args);
}

Exception::Exception(const char * message)
{
	this->message = (string)message;
}

Exception::Exception(lua_State * L)
{
	if (!lua_isnil(L, -1))
		this->message = (string)lua_tostring(L, -1);
}

Exception::~Exception() throw()
{

}