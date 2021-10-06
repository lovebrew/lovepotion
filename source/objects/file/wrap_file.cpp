#include "objects/file/wrap_file.h"

using namespace love;

int Wrap_File::Close(lua_State* L)
{
    File* self = Wrap_File::CheckFile(L, 1);

    lua_pushboolean(L, self->Close());

    return 1;
}

int Wrap_File::Flush(lua_State* L)
{
    File* self   = Wrap_File::CheckFile(L, 1);
    bool success = false;

    try
    {
        success = self->Flush();
    }
    catch (love::Exception& e)
    {
        return Luax::IOError(L, "%s", e.what());
    }

    lua_pushboolean(L, success);

    return 1;
}

int Wrap_File::GetBuffer(lua_State* L)
{
    File* self = Wrap_File::CheckFile(L, 1);

    int64_t size;
    File::BufferMode mode;

    mode               = self->GetBuffer(size);
    const char* string = 0;

    if (!File::GetConstant(mode, string))
        return Luax::IOError(L, "Unknown file buffer mode.");

    lua_pushstring(L, string);
    lua_pushnumber(L, (lua_Number)size);

    return 2;
}

int Wrap_File::GetFilename(lua_State* L)
{
    File* self = Wrap_File::CheckFile(L, 1);

    lua_pushstring(L, self->GetFilename().c_str());

    return 1;
}

int Wrap_File::GetMode(lua_State* L)
{
    File* self = Wrap_File::CheckFile(L, 1);

    File::Mode mode    = self->GetMode();
    const char* string = 0;

    if (!File::GetConstant(mode, string))
        return Luax::IOError(L, "Unknown file mode.");

    lua_pushstring(L, string);

    return 1;
}

int Wrap_File::GetSize(lua_State* L)
{
    File* self   = Wrap_File::CheckFile(L, 1);
    int64_t size = -1;

    try
    {
        size = self->GetSize();
    }
    catch (love::Exception& e)
    {
        return Luax::IOError(L, e.what());
    }

    if (size == -1)
        return Luax::IOError(L, "Could not determine file size.");
    else if (size >= 0x20000000000000LL)
        return Luax::IOError(L, "Size is too large.");

    lua_pushnumber(L, (lua_Number)size);

    return 1;
}

int Wrap_File::IsEOF(lua_State* L)
{
    File* self = Wrap_File::CheckFile(L, 1);

    lua_pushboolean(L, self->IsEOF());

    return 1;
}

int Wrap_File::IsOpen(lua_State* L)
{
    File* self = Wrap_File::CheckFile(L, 1);

    lua_pushboolean(L, self->IsOpen());

    return 1;
}

/*
** See: https://github.com/love2d/love/blob/master/src/modules/filesystem/wrap_File.cpp#L239
**/
int Wrap_File::Lines_I(lua_State* L)
{
    /*
    ** The upvalues:
    ** File
    ** read buffer (string)
    ** read buffer offset (number)
    ** file position (number, optional)
    ** restore userpos (bool, optional)
    */

    File* self = Luax::CheckType<File>(L, lua_upvalueindex(1));

    if (self->GetMode() != File::MODE_READ)
        return luaL_error(L, "File needs to stay in read mode.");

    size_t length;
    const char* buffer = lua_tolstring(L, lua_upvalueindex(2), &length);
    int offset         = lua_tointeger(L, lua_upvalueindex(3));

    const char* start = buffer + offset;
    const char* end   = reinterpret_cast<const char*>(memchr(start, '\n', length - offset));

    bool seekBack = lua_toboolean(L, lua_upvalueindex(5));

    while (!end && !self->IsEOF())
    {
        const int bufferSize = 1024;
        char readBuffer[bufferSize];

        // new buffer
        luaL_Buffer storage;
        luaL_buffinit(L, &storage);
        luaL_addlstring(&storage, start, length - offset);

        // seek back if the user changed the position
        int64_t position    = self->Tell();
        int64_t userPostion = -1;

        if (seekBack)
        {
            userPostion = position;
            position    = (int64_t)lua_tonumber(L, lua_upvalueindex(4));

            if (userPostion != position)
                self->Seek(position);
        }

        // keep reading until EOF
        while (!self->IsEOF())
        {
            int read = (int)self->Read(readBuffer, bufferSize);

            if (read < 0)
                return luaL_error(L, "Could not read from file.");

            luaL_addlstring(&storage, readBuffer, read);

            // break at a newline, if found
            if (memchr(readBuffer, '\n', read))
                break;
        }

        // possibly seek back and save our target position too
        if (seekBack)
        {
            lua_pushnumber(L, self->Tell());
            lua_replace(L, lua_upvalueindex(4));

            self->Seek(userPostion);
        }

        luaL_pushresult(&storage);
        lua_replace(L, lua_upvalueindex(2));

        buffer = lua_tolstring(L, lua_upvalueindex(2), &length);
        offset = 0;
        start  = buffer;
        end    = reinterpret_cast<const char*>(memchr(start, '\n', length));
    }

    if (!end)
        end = buffer + length - 1;

    offset = end - buffer + 1;
    lua_pushinteger(L, offset);
    lua_replace(L, lua_upvalueindex(3));

    if (start == buffer + length)
    {
        self->Close();

        return 0;
    }

    if (end >= start && *end == '\n')
        --end;

    if (end >= start && *end == '\r')
        --end;

    lua_pushlstring(L, start, end - start + 1);

    return 1;
}

/*
** See: https://github.com/love2d/love/blob/master/src/modules/filesystem/wrap_File.cpp#L347
*/
int Wrap_File::Lines(lua_State* L)
{
    File* self = Wrap_File::CheckFile(L, 1);

    lua_pushstring(L, "");
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    lua_pushboolean(L, self->GetMode() != File::MODE_CLOSED);

    File::Mode currentMode = self->GetMode();

    if (currentMode != File::MODE_READ)
    {
        if (currentMode != File::MODE_CLOSED)
            self->Close();

        bool success = false;

        Luax::CatchException(L, [&]() { success = self->Open(File::MODE_READ); });

        if (!success)
            return luaL_error(L, "Could not open file.");
    }

    lua_pushcclosure(L, Wrap_File::Lines_I, 5);

    return 1;
}

int Wrap_File::Open(lua_State* L)
{
    File* self = Wrap_File::CheckFile(L, 1);

    const char* string = luaL_checkstring(L, 2);
    File::Mode mode;

    if (!File::GetConstant(string, mode))
        Luax::EnumError(L, "file open mode", File::GetConstants(mode), string);

    try
    {
        lua_pushboolean(L, self->Open(mode));
    }
    catch (love::Exception& e)
    {
        return Luax::IOError(L, "%s", e.what());
    }

    return 1;
}

int Wrap_File::Read(lua_State* L)
{
    File* self                     = Wrap_File::CheckFile(L, 1);
    StrongReference<FileData> data = nullptr;

    love::data::ContainerType type = love::data::CONTAINER_STRING;
    int start                      = 2;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        type  = Wrap_DataModule::CheckContainerType(L, 2);
        start = 3;
    }

    int64_t size = (int64_t)luaL_optnumber(L, start, (lua_Number)File::ALL);

    try
    {
        data.Set(self->Read(size), Acquire::NORETAIN);
    }
    catch (love::Exception& e)
    {
        return Luax::IOError(L, "%s", e.what());
    }

    if (type == love::data::CONTAINER_DATA)
        Luax::PushType(L, data.Get());
    else
        lua_pushlstring(L, (const char*)data->GetData(), data->GetSize());

    lua_pushinteger(L, data->GetSize());

    return 2;
}

int Wrap_File::Seek(lua_State* L)
{
    File* self = Wrap_File::CheckFile(L, 1);

    lua_Number position = luaL_checknumber(L, 2);

    if (position < 0 || position > 9007199254740992)
        lua_pushboolean(L, false);
    else
        lua_pushboolean(L, self->Seek((uint64_t)position));

    return 1;
}

int Wrap_File::SetBuffer(lua_State* L)
{
    File* self      = Wrap_File::CheckFile(L, 1);
    const char* str = luaL_checkstring(L, 2);
    int64_t size    = (int64_t)luaL_optnumber(L, 3, 0.0);

    File::BufferMode bufferMode;
    if (!File::GetConstant(str, bufferMode))
        return Luax::EnumError(L, "file buffer mode", File::GetConstants(bufferMode), str);

    bool success = false;

    try
    {
        success = self->SetBuffer(bufferMode, size);
    }
    catch (love::Exception& e)
    {
        return Luax::IOError(L, "%s", e.what());
    }

    lua_pushboolean(L, success);

    return 1;

    return 1;
}

int Wrap_File::Tell(lua_State* L)
{
    File* self = Wrap_File::CheckFile(L, 1);

    int64_t position = self->Tell();

    if (position == -1)
        return Luax::IOError(L, "Invalid position.");
    else if (position >= 0x20000000000000LL)
        return Luax::IOError(L, "Number is too large.");
    else
        lua_pushnumber(L, (lua_Number)position);

    return 1;
}

int Wrap_File::Write(lua_State* L)
{
    File* self   = Wrap_File::CheckFile(L, 1);
    bool success = false;

    if (lua_isstring(L, 2))
    {
        try
        {
            size_t dataSize  = 0;
            const char* data = lua_tolstring(L, 2, &dataSize);

            if (!lua_isnoneornil(L, 3))
                dataSize = luaL_checkinteger(L, 3);

            success = self->Write(data, dataSize);
        }
        catch (love::Exception& e)
        {
            Luax::IOError(L, "%s", e.what());
        }
    }
    else if (Luax::IsType(L, 2, love::Data::type))
    {
        try
        {
            Data* data = Luax::ToType<Data>(L, 2);

            int64_t size = luaL_optinteger(L, 3, data->GetSize());
            success      = self->Write(data, size);
        }
        catch (love::Exception& e)
        {
            return Luax::IOError(L, "%s", e.what());
        }
    }
    else
        return luaL_argerror(L, 2, "string or data expected.");

    lua_pushboolean(L, success);

    return 1;
}

File* Wrap_File::CheckFile(lua_State* L, int index)
{
    return Luax::CheckType<File>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "close",       Wrap_File::Close       },
    { "flush",       Wrap_File::Flush       },
    { "getBuffer",   Wrap_File::GetBuffer   },
    { "getFilename", Wrap_File::GetFilename },
    { "getMode",     Wrap_File::GetMode     },
    { "getSize",     Wrap_File::GetSize     },
    { "isEOF",       Wrap_File::IsEOF       },
    { "isOpen",      Wrap_File::IsOpen      },
    { "lines",       Wrap_File::Lines       },
    { "open",        Wrap_File::Open        },
    { "read",        Wrap_File::Read        },
    { "seek",        Wrap_File::Seek        },
    { "setBuffer",   Wrap_File::SetBuffer   },
    { "tell",        Wrap_File::Tell        },
    { "write",       Wrap_File::Write       },
    { 0,             0                      }
};
// clang-format on

int Wrap_File::Register(lua_State* L)
{
    return Luax::RegisterType(L, &File::type, functions, nullptr);
}
