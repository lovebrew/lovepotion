#include "common/runtime.h"

#include "common/drawable.h"

#include "objects/font/font.h"
#include "objects/text/text.h"
#include "objects/text/wrap_text.h"

#define CLASS_TYPE LUAOBJ_TYPE_TEXT
#define CLASS_NAME "Text"

int textNew(lua_State * L)
{
    return 0;
}

int textToString(lua_State * L)
{
    Text * self = (Text *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString();

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int initTextClass(lua_State * L)
{
    return 0;
}