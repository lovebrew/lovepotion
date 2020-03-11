#include "common/runtime.h"
#include "modules/filesystem/wrap_filesystem.h"

using namespace love;

#define instance() (Module::GetInstance<Filesystem>(Module::M_FILESYSTEM))

int Wrap_Filesystem::Append(lua_State * L)
{
    return Wrap_Filesystem::WriteOrAppend(L, File::MODE_APPEND);
}

int Wrap_Filesystem::CreateDirectory(lua_State * L)
{
    const char * name = luaL_checkstring(L, 1);

    instance()->CreateDirectory(name);

    return 0;
}

int Wrap_Filesystem::GetDirectoryItems(lua_State * L)
{
    const char * directory = luaL_checkstring(L, 1);
    std::vector<std::string> items;

    instance()->GetDirectoryItems(directory, items);

    lua_createtable(L, (int)items.size(), 0);

    for (size_t index = 0; index < items.size(); index++)
    {
        lua_pushlstring(L, items[index].data(), items[index].size());
        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_Filesystem::GetIdentity(lua_State * L)
{
    std::string identity = instance()->GetIdentity();

    lua_pushlstring(L, identity.data(), identity.size());

    return 1;
}

int Wrap_Filesystem::GetInfo(lua_State * L)
{
    const char * filepath = luaL_checkstring(L, 1);
    Filesystem::Info info = {};

    int start = 2;
    Filesystem::FileType filter = Filesystem::FileType::MAX_ENUM;

    if (lua_isstring(L, start))
    {
        const char * type = luaL_checkstring(L, start);
        if (!Filesystem::GetConstant(type, filter))
            return 0; // TO DO enumerror

        start++;
    }

    if (instance()->GetInfo(filepath, info))
    {
        if (filter != Filesystem::FileType::MAX_ENUM && info.type != filter)
        {
            lua_pushnil(L);

            return 1;
        }

        const char * type = nullptr;
        if (!Filesystem::GetConstant(info.type, type))
            return luaL_error(L, "Uknown file type.");

        if (lua_istable(L, start))
            lua_pushvalue(L, start);
        else
            lua_createtable(L, 0, 3);

        lua_pushstring(L, type);
        lua_setfield(L, -2, "type");

        if (info.size >= 0)
        {
            lua_pushnumber(L, (lua_Number)info.size);
            lua_setfield(L, -2, "size");
        }

        if (info.modtime >= 0)
        {
            lua_pushnumber(L, (lua_Number)info.modtime);
            lua_setfield(L, -2, "modtime");
        }
    }
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Filesystem::GetSaveDirectory(lua_State * L)
{
    std::string saveDirectory = instance()->GetSaveDirectory();

    lua_pushlstring(L, saveDirectory.data(), saveDirectory.size());

    return 1;
}

int Wrap_Filesystem::Load(lua_State * L)
{
    std::string filename = luaL_checkstring(L, 1);
    Data * data = nullptr;

    try
    {
        data = instance()->Read(filename.c_str());
    }
    catch(love::Exception & e)
    {
        return Luax::IOError(L, "%s", e.what());
    }

    int status = luaL_loadbuffer(L, (const char *)data->GetData(), data->GetSize(), ("@" + filename).c_str());
    data->Release();

    switch (status)
    {
        case LUA_ERRMEM:
            return luaL_error(L, "Memory allocation error: %s\n", lua_tostring(L, -1));
        case LUA_ERRSYNTAX:
            return luaL_error(L, "Syntax error: %s\n", lua_tostring(L, -1));
        default:
            return 1;
    }
}

int Wrap_Filesystem::NewFile(lua_State * L)
{
    const char * filename = luaL_checkstring(L, 1);

    const char * string = 0;
    File::Mode mode = File::MODE_CLOSED;

    if (lua_isstring(L, 2))
    {
        string = luaL_checkstring(L, 2);
        if (!File::GetConstant(string, mode))
            return Luax::EnumError(L, "file open mode", File::GetConstants(mode), string);
    }

    File * file = instance()->NewFile(filename);

    if (mode != File::MODE_CLOSED)
    {
        try
        {
            if (!file->Open(mode))
                throw love::Exception("Could not open file.");
        }
        catch(love::Exception & e)
        {
            file->Release();
            return Luax::IOError(L, "%s", e.what());
        }
    }

    Luax::PushType(L, file);
    file->Release();

    return 1;
}

int Wrap_Filesystem::NewFileData(lua_State * L)
{
    if (lua_gettop(L) == 1)
    {
        if (lua_isstring(L, 1))
            Luax::ConvertObject(L, 1, "filesystem", "newFile");

        if (Luax::IsType(L, 1, File::type))
        {
            File * file = Wrap_File::CheckFile(L, 1);
            StrongReference<FileData> data;

            try
            {
                data.Set(file->Read(), Acquire::NORETAIN);
            }
            catch(love::Exception & e)
            {
                return Luax::IOError(L, "%s", e.what());
            }

            Luax::PushType(L, data);

            return 1;
        }
        else
            return luaL_argerror(L, 1, "filename or File expected");
    }

    size_t length = 0;
    const char * string = luaL_checklstring(L, 1, &length);
    const char * filename = luaL_checkstring(L, 2);

    FileData * data = nullptr;
    Luax::CatchException(L, [&]() {
        data = instance()->NewFileData(string, length, filename);
    });

    Luax::PushType(L, data);
    data->Release();

    return 1;
}

int Wrap_Filesystem::Read(lua_State * L)
{
    love::data::ContainerType type = love::data::CONTAINER_STRING;
    int start = 1;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        type = Wrap_DataModule::CheckContainerType(L, 1);
        start = 2;
    }

    const char * filename = luaL_checkstring(L, start + 0);
    int64_t length = (int64_t)luaL_optinteger(L, start + 1, File::ALL);

    FileData * data = nullptr;

    try
    {
        data = instance()->Read(filename, length);
    }
    catch(love::Exception & e)
    {
        return Luax::IOError(L, "%s", e.what());
    }

    if (data == nullptr)
        return Luax::IOError(L, "File could not be read.");

    if (type == love::data::CONTAINER_DATA)
        Luax::PushType(L, data);
    else
        lua_pushlstring(L, (const char *)data->GetData(), data->GetSize());

    lua_pushinteger(L, data->GetSize());
    data->Release();

    return 2;
}

int Wrap_Filesystem::Remove(lua_State * L)
{
    const char * filename = luaL_checkstring(L, 1);

    bool success = instance()->Remove(filename);

    lua_pushboolean(L, success);

    return 1;
}

int Wrap_Filesystem::SetIdentity(lua_State * L)
{
    const char * name = luaL_checkstring(L, 1);

    instance()->SetIdentity(name);

    return 0;
}

int Wrap_Filesystem::WriteOrAppend(lua_State * L, File::Mode mode)
{
    const char * filename = luaL_checkstring(L, 1);

    const char * input = nullptr;
    size_t length = 0;

    if (Luax::IsType(L, 2, Data::type))
    {
        Data * data = Luax::ToType<Data>(L, 2);

        input = (const char *)data->GetData();
        length = data->GetSize();
    }
    else if (lua_isstring(L, 2))
        input = lua_tolstring(L, 2, &length);
    else
        return luaL_argerror(L, 2, "string or Data expected");

    length = luaL_optinteger(L, 3, length);

    try
    {
        if (mode == File::MODE_APPEND)
            instance()->Append(filename, (const void *)input, length);
        else
            instance()->Write(filename, (const void *)input, length);
    }
    catch(love::Exception & e)
    {
        return Luax::IOError(L, "%s", e.what());
    }

    lua_pushboolean(L, true);

    return 1;
}

int Wrap_Filesystem::Write(lua_State * L)
{
    return Wrap_Filesystem::WriteOrAppend(L, File::MODE_WRITE);
}

File * Wrap_Filesystem::GetFile(lua_State * L, int index)
{
    File * file = nullptr;

    if (lua_isstring(L, index))
    {
        const char * filename = luaL_checkstring(L, index);
        file = instance()->NewFile(filename);
    }
    else
    {
        file = Wrap_File::CheckFile(L, index);
        file->Retain();
    }

    return file;
}

FileData * Wrap_Filesystem::GetFileData(lua_State * L, int index)
{
    FileData * data = nullptr;
    File * file = nullptr;

    if (lua_isstring(L, index) || Luax::IsType(L, index, File::type))
        file = Wrap_Filesystem::GetFile(L, index);
    else if (Luax::IsType(L, index, FileData::type))
    {
        data = Wrap_FileData::CheckFileData(L, index);
        data->Retain();
    }

    if (!data && !file)
    {
        luaL_argerror(L, index, "filename, File, or FileData expected.");
        return nullptr;
    }

    if (file)
    {
        Luax::CatchException(L,
            [&]() { data = file->Read(); },
            [&](bool) { file->Release(); }
        );
    }

    return data;
}

int Wrap_Filesystem::Register(lua_State * L)
{
    luaL_reg reg[] =
    {
        { "createDirectory",        CreateDirectory   },
        { "getDirectoryItems",      GetDirectoryItems },
        { "getIdentity",            GetIdentity       },
        { "getInfo",                GetInfo           },
        { "getSaveDirectory",       GetSaveDirectory  },
        { "load",                   Load              },
        { "newFile",                NewFile           },
        { "newFileData",            NewFileData       },
        { "read",                   Read              },
        { "remove",                 Remove            },
        { "setIdentity",            SetIdentity       },
        { "write",                  Write             },
        { 0,                        0                 }
    };

    lua_CFunction types[] =
    {
        Wrap_FileData::Register,
        Wrap_File::Register,
        0
    };

    Filesystem * instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Filesystem(); });
    else
        instance->Retain();

    WrappedModule module;

    module.instance = instance;
    module.name = "filesystem";
    module.functions = reg;
    module.type = &Module::type;
    module.types = types;

    return Luax::RegisterModule(L, module);
}
