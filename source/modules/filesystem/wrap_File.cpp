#include "modules/filesystem/wrap_File.hpp"

#include "common/Data.hpp"
#include "common/Exception.hpp"
#include "common/int.hpp"

#include "modules/data/wrap_DataModule.hpp"

using namespace love;

int Wrap_File::getSize(lua_State* L)
{
    auto* self   = luax_checkfile(L, 1);
    int64_t size = -1;

    try
    {
        size = self->getSize();
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    if (size == -1)
        return luax_ioerror(L, "Could not determine file size.");
    else if (size >= File::MAX_FILE_SIZE)
        return luax_ioerror(L, "Size is too large.");

    lua_pushnumber(L, (lua_Number)size);

    return 1;
}

int Wrap_File::open(lua_State* L)
{
    auto* self             = luax_checkfile(L, 1);
    const char* modeString = luaL_checkstring(L, 2);

    File::Mode mode = File::MODE_MAX_ENUM;
    if (!File::getConstant(modeString, mode))
        return luax_enumerror(L, "file open mode", File::OpenModes, modeString);

    try
    {
        luax_pushboolean(L, self->open(mode));
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    return 1;
}

int Wrap_File::close(lua_State* L)
{
    auto* self = luax_checkfile(L, 1);

    luax_pushboolean(L, self->close());

    return 1;
}

int Wrap_File::isOpen(lua_State* L)
{
    auto* self = luax_checkfile(L, 1);

    luax_pushboolean(L, self->isOpen());

    return 1;
}

int Wrap_File::read(lua_State* L)
{
    auto* self               = luax_checkfile(L, 1);
    StrongRef<FileData> data = nullptr;

    auto containerType = data::CONTAINER_STRING;
    int start          = 2;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        containerType = luax_checkcontainertype(L, 2);
        start         = 3;
    }

    int64_t size = (int64_t)luaL_optnumber(L, start, -1);

    try
    {
        if (size < 0)
            size = self->getSize();

        data.set(self->read(size), Acquire::NO_RETAIN);
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    if (containerType == data::CONTAINER_DATA)
        luax_pushtype(L, data.get());
    else
        lua_pushlstring(L, (const char*)data->getData(), (size_t)data->getSize());

    lua_pushinteger(L, (lua_Integer)data->getSize());

    return 2;
}

int Wrap_File::write(lua_State* L)
{
    auto* self  = luax_checkfile(L, 1);
    bool result = false;

    if (lua_isstring(L, 2))
    {
        try
        {
            size_t dataSize  = 0;
            const char* data = lua_tolstring(L, 2, &dataSize);

            if (!lua_isnoneornil(L, 3))
                dataSize = luaL_checkinteger(L, 3);

            result = self->write(data, dataSize);
        }
        catch (love::Exception& e)
        {
            return luax_ioerror(L, "%s", e.what());
        }
    }
    else if (luax_istype(L, 2, Data::type))
    {
        try
        {
            auto* data      = luax_totype<Data>(L, 2);
            const auto size = luaL_optinteger(L, 3, data->getSize());

            result = self->write(data->getData(), size);
        }
        catch (love::Exception& e)
        {
            return luax_ioerror(L, "%s", e.what());
        }
    }
    else
        return luaL_argerror(L, 2, "string or Data expected");

    luax_pushboolean(L, result);

    return 1;
}

int Wrap_File::flush(lua_State* L)
{
    auto* self   = luax_checkfile(L, 1);
    bool success = false;

    try
    {
        success = self->flush();
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    luax_pushboolean(L, success);

    return 1;
}

int Wrap_File::isEOF(lua_State* L)
{
    auto* self = luax_checkfile(L, 1);

    luax_pushboolean(L, self->isEOF());

    return 1;
}

int Wrap_File::tell(lua_State* L)
{
    auto* self       = luax_checkfile(L, 1);
    int64_t position = self->tell();

    if (position == -1)
        return luax_ioerror(L, "Invalid position.");
    else if (position >= File::MAX_FILE_SIZE)
        return luax_ioerror(L, "Number is too large.");
    else
        lua_pushnumber(L, (lua_Number)position);

    return 1;
}

int Wrap_File::seek(lua_State* L)
{
    auto* self          = luax_checkfile(L, 1);
    lua_Number position = luaL_checknumber(L, 2);

    if (position < 0.0 || position >= 9007199254740992.0)
        luax_pushboolean(L, false);
    else
        luax_pushboolean(L, self->seek((uint64_t)position));

    return 1;
}

int Wrap_File::lines_i(lua_State* L)
{
    auto* self = luax_checktype<File>(L, lua_upvalueindex(1));

    if (self->getMode() != File::MODE_READ)
        return luaL_error(L, "File needs to stay in read mode.");

    size_t length      = 0;
    const char* buffer = lua_tolstring(L, lua_upvalueindex(2), &length);
    int offset         = lua_tointeger(L, lua_upvalueindex(3));

    const char* start = buffer + offset;
    const char* end   = reinterpret_cast<const char*>(memchr(start, '\n', length - offset));

    bool seekBack = luax_toboolean(L, lua_upvalueindex(5));

    if (!end && !self->isEOF())
    {
        const int readSize = 0x400;
        char readBuffer[readSize] { '\0' };

        luaL_Buffer storage;
        luaL_buffinit(L, &storage);
        luaL_addlstring(&storage, start, length - offset);

        int64_t position     = self->tell();
        int64_t userPosition = -1;

        if (seekBack)
        {
            userPosition = position;
            position     = (int64_t)lua_tonumber(L, lua_upvalueindex(4));

            if (userPosition != position)
                self->seek(position);
        }

        while (!self->isEOF())
        {
            int read = (int)self->read(readBuffer, readSize);
            if (read < 0)
                return luaL_error(L, "Could not read from file.");

            luaL_addlstring(&storage, readBuffer, read);

            if (memchr(readBuffer, '\n', read))
                break;
        }

        if (seekBack)
        {
            lua_pushnumber(L, self->tell());
            lua_replace(L, lua_upvalueindex(4));
            self->seek(userPosition);
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
        self->close();
        return 0;
    }

    if (end >= start && *end == '\n')
        --end;

    if (end >= start && *end == '\r')
        --end;

    lua_pushlstring(L, start, end - start + 1);

    return 1;
}

int Wrap_File::lines(lua_State* L)
{
    auto* self = luax_checkfile(L, 1);

    lua_pushstring(L, "");
    lua_pushnumber(L, 0);
    lua_pushnumber(L, 0);
    luax_pushboolean(L, self->getMode() != File::MODE_CLOSED);

    if (self->getMode() != File::MODE_READ)
    {
        if (self->getMode() != File::MODE_CLOSED)
            self->close();

        bool success = false;
        luax_catchexcept(L, [&] { success = self->open(File::MODE_READ); });

        if (!success)
            return luaL_error(L, "Could not open file.");
    }

    lua_pushcclosure(L, Wrap_File::lines_i, 5);

    return 1;
}

int Wrap_File::setBuffer(lua_State* L)
{
    auto* self = luax_checkfile(L, 1);

    const char* string = luaL_checkstring(L, 2);
    int64_t size       = (int64_t)luaL_optnumber(L, 3, 0.0);

    File::BufferMode mode = File::BUFFER_MAX_ENUM;
    if (!File::getConstant(string, mode))
        return luax_enumerror(L, "file buffer mode", File::BufferModes, string);

    bool success = false;

    try
    {
        success = self->setBuffer(mode, size);
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    luax_pushboolean(L, success);

    return 1;
}

int Wrap_File::getBuffer(lua_State* L)
{
    auto* self   = luax_checkfile(L, 1);
    int64_t size = 0;

    auto mode = self->getBuffer(size);
    std::string_view modeString {};

    if (!File::getConstant(mode, modeString))
        return luax_ioerror(L, "Unknown file buffer mode.");

    luax_pushstring(L, modeString);
    lua_pushnumber(L, (lua_Number)size);

    return 2;
}

int Wrap_File::getMode(lua_State* L)
{
    auto* self = luax_checkfile(L, 1);

    const auto mode = self->getMode();
    std::string_view modeString {};

    if (!File::getConstant(mode, modeString))
        return luax_ioerror(L, "Unknown file mode.");

    luax_pushstring(L, modeString);

    return 1;
}

int Wrap_File::getFilename(lua_State* L)
{
    auto* self = luax_checkfile(L, 1);

    luax_pushstring(L, self->getFilename());

    return 1;
}

int Wrap_File::getExtension(lua_State* L)
{
    auto* self = luax_checkfile(L, 1);

    luax_pushstring(L, self->getExtension());

    return 1;
}

// clang-format off
const luaL_Reg Wrap_File::w_File_functions[16] =
{
    { "getSize",      Wrap_File::getSize      },
    { "open",         Wrap_File::open         },
    { "close",        Wrap_File::close        },
    { "isOpen",       Wrap_File::isOpen       },
    { "read",         Wrap_File::read         },
    { "write",        Wrap_File::write        },
    { "flush",        Wrap_File::flush        },
    { "isEOF",        Wrap_File::isEOF        },
    { "tell",         Wrap_File::tell         },
    { "seek",         Wrap_File::seek         },
    { "lines",        Wrap_File::lines        },
    { "setBuffer",    Wrap_File::setBuffer    },
    { "getBuffer",    Wrap_File::getBuffer    },
    { "getMode",      Wrap_File::getMode      },
    { "getFilename",  Wrap_File::getFilename  },
    { "getExtension", Wrap_File::getExtension }
};
// clang-format on

namespace love
{
    File* luax_checkfile(lua_State* L, int index)
    {
        return luax_checktype<File>(L, index);
    }

    int open_file(lua_State* L)
    {
        return luax_register_type(L, &File::type, Wrap_File::w_File_functions);
    }
} // namespace love
