#include <modules/data/data.hpp>
#include <modules/data/wrap_data.hpp>

#include <modules/filesystem/physfs/filesystem.hpp>
#include <modules/filesystem/wrap_filesystem.hpp>

#include <objects/data/filedata/wrap_filedata.hpp>
#include <objects/file/wrap_file.hpp>

#include <filesystem>
#include <format>

#include <utilities/functions.hpp>

using namespace love;

#define instance() (Module::GetInstance<Filesystem>(Module::M_FILESYSTEM))

std::size_t replaceAll(std::string& inout, std::string_view what, std::string_view with)
{
    std::size_t count {};
    for (std::string::size_type pos {};
         inout.npos != (pos = inout.find(what.data(), pos, what.length()));
         pos += with.length(), ++count)
    {
        inout.replace(pos, what.length(), with.data(), with.length());
    }
    return count;
}

Filesystem::MountPermissions Wrap_Filesystem::CheckPermissionType(lua_State* L, int index)
{
    const char* string = luaL_checkstring(L, index);

    if (auto found = Filesystem::mountPermissions.Find(string))
        return *found;
    else
        luax::EnumError(L, "mount permissions", Filesystem::mountPermissions, string);

    return Filesystem::MountPermissions::MOUNT_READ;
}

Filesystem::CommonPath Wrap_Filesystem::CheckCommonPathType(lua_State* L, int index)
{
    const char* string = luaL_checkstring(L, index);

    if (auto found = Filesystem::commonPaths.Find(string))
        return *found;
    else
        luax::EnumError(L, "mount permissions", Filesystem::commonPaths, string);

    return Filesystem::CommonPath::APP_SAVEDIR;
}

int Wrap_Filesystem::Load(lua_State* L)
{
    std::string filename = luaL_checkstring(L, 1);
    Data* data           = nullptr;

    try
    {
        data = instance()->Read(filename.c_str());
    }
    catch (love::Exception& e)
    {
        return luax::IOError(L, "%s", e.what());
    }

    int status =
        luaL_loadbuffer(L, (const char*)data->GetData(), data->GetSize(), ("@" + filename).c_str());
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

int Wrap_Filesystem::Init(lua_State* L)
{
    const char* arg0 = luaL_optstring(L, 1, NULL);

    luax::CatchException(L, [&]() { instance()->Init(arg0); });

    return 0;
}

int Wrap_Filesystem::Mount(lua_State* L)
{
    std::string archive {};

    if (luax::IsType(L, 1, Data::type))
    {
        Data* data = Wrap_Data::CheckData(L, 1);
        int start  = 2;

        if (luax::IsType(L, 1, FileData::type) && !lua_isstring(L, 3))
        {
            FileData* fileData = Wrap_FileData::CheckFileData(L, 1);
            archive            = fileData->GetFilename();
            start              = 2;
        }
        else
        {
            archive = luaL_checkstring(L, 2);
            start   = 3;
        }

        const char* mountPoint = luaL_checkstring(L, start + 0);
        bool append            = luax::OptBoolean(L, start + 1, false);

        luax::PushBoolean(L, instance()->Mount(data, archive.c_str(), mountPoint, append));

        return 1;
    }
    else // Don't check for DroppedFile (as LOVE would) since it's not supported here
        archive = luaL_checkstring(L, 1);

    const char* mountPoint = luaL_checkstring(L, 2);
    bool append            = luax::OptBoolean(L, 3, false);

    luax::PushBoolean(L, instance()->Mount(archive.c_str(), mountPoint, append));

    return 1;
}

int Wrap_Filesystem::MountFullPath(lua_State* L)
{
    const char* fullPath   = luaL_checkstring(L, 1);
    const char* mountPoint = luaL_checkstring(L, 2);

    auto permissions = Filesystem::MOUNT_READ;

    if (!lua_isnoneornil(L, 3))
        permissions = Wrap_Filesystem::CheckPermissionType(L, 3);

    bool append = luax::OptBoolean(L, 4, false);

    luax::PushBoolean(L, instance()->MountFullPath(fullPath, mountPoint, permissions, append));

    return 1;
}

int Wrap_Filesystem::MountCommonPath(lua_State* L)
{
    auto commonPath        = Wrap_Filesystem::CheckCommonPathType(L, 1);
    const char* mountPoint = luaL_checkstring(L, 2);
    auto permissions       = Wrap_Filesystem::CheckPermissionType(L, 3);
    bool append            = luax::OptBoolean(L, 4, false);

    luax::PushBoolean(L, instance()->MountCommonPath(commonPath, mountPoint, permissions, append));

    return 1;
}

int Wrap_Filesystem::UnMount(lua_State* L)
{
    if (luax::IsType(L, 1, Data::type))
    {
        Data* data = Wrap_Data::CheckData(L, 1);
        lua_pushboolean(L, instance()->UnMount(data));
    }
    else
    {
        const char* archive = luaL_checkstring(L, 1);
        lua_pushboolean(L, instance()->UnMount(archive));
    }

    return 1;
}

int Wrap_Filesystem::UnMountFullPath(lua_State* L)
{
    const char* fullPath = luaL_checkstring(L, 1);

    luax::PushBoolean(L, instance()->UnMountFullPath(fullPath));

    return 1;
}

int Wrap_Filesystem::UnMountCommonPath(lua_State* L)
{
    auto commonPath = Wrap_Filesystem::CheckCommonPathType(L, 1);

    luax::PushBoolean(L, instance()->UnMount(commonPath));

    return 1;
}

int Wrap_Filesystem::GetUserDirectory(lua_State* L)
{
    auto directory = instance()->GetUserDirectory();

    luax::PushString(L, directory);

    return 1;
}

int Wrap_Filesystem::Loader(lua_State* L)
{
    std::string moduleName = luaL_checkstring(L, 1);

    for (char& c : moduleName)
    {
        if (c == '.')
            c = '/';
    }

    auto* inst = instance();
    for (std::string element : inst->GetRequirePath())
    {
        replaceAll(element, "?", moduleName);

        Filesystem::Info info {};

        if (inst->GetInfo(element.c_str(), info))
        {
            if (info.type != Filesystem::FILETYPE_DIRECTORY)
            {
                lua_pop(L, 1);
                lua_pushstring(L, element.c_str());

                return Wrap_Filesystem::Load(L);
            }
        }
    }

    std::string errstr = "\n    no '%s' in LOVE game directories.";

    lua_pushfstring(L, errstr.c_str(), moduleName.c_str());

    return 1;
}

bool Wrap_Filesystem::SetupWriteDirectory()
{
    if (instance() != 0)
        return instance()->SetupWriteDirectory();

    return false;
}

int Wrap_Filesystem::GetRequirePath(lua_State* L)
{
    std::string path;
    bool seperator = false;
    auto paths     = instance()->GetRequirePath();

    for (auto& element : paths)
    {
        if (seperator)
            path += ";";
        else
            seperator = true;

        path += element;
    }

    lua_pushlstring(L, path.data(), path.size());

    return 1;
}

int Wrap_Filesystem::Append(lua_State* L)
{
    return Wrap_Filesystem::WriteOrAppend(L, File::MODE_APPEND);
}

int Wrap_Filesystem::CreateDirectory(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);

    bool success = instance()->CreateDirectory(name);

    lua_pushboolean(L, success);

    return 1;
}

int Wrap_Filesystem::SetFused(lua_State* L)
{
    instance()->SetFused(lua_toboolean(L, 1));

    return 0;
}

int Wrap_Filesystem::GetExecutablePath(lua_State* L)
{
    luax::PushString(L, instance()->GetExecutablePath());

    return 1;
}

int Wrap_Filesystem::IsFused(lua_State* L)
{
    lua_pushboolean(L, instance()->IsFused());

    return 1;
}

int Wrap_Filesystem::SetSource(lua_State* L)
{
    const char* arg = luaL_checkstring(L, 1);

    if (!instance()->SetSource(arg))
        return luaL_error(L, "Could not set source.");

    return 0;
}

int Wrap_Filesystem::GetSource(lua_State* L)
{
    luax::PushString(L, instance()->GetSource());

    return 1;
}

int Wrap_Filesystem::GetDirectoryItems(lua_State* L)
{
    const char* directory = luaL_checkstring(L, 1);
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

int Wrap_Filesystem::GetIdentity(lua_State* L)
{
    luax::PushString(L, instance()->GetIdentity());

    return 1;
}

int Wrap_Filesystem::Exists(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);

    lua_pushboolean(L, instance()->Exists(filename));

    return 1;
}

int Wrap_Filesystem::GetInfo(lua_State* L)
{
    const char* filepath = luaL_checkstring(L, 1);
    Filesystem::Info info {};

    int start   = 2;
    auto filter = Filesystem::FILETYPE_MAX_ENUM;

    if (lua_isstring(L, start))
    {
        const char* type = luaL_checkstring(L, start);
        if (auto found = Filesystem::fileTypes.Find(type))
            filter = *found;
        else
            return luax::EnumError(L, "file type", Filesystem::fileTypes, type);

        start++;
    }

    std::filesystem::path path = filepath;
    translatePath(path);

    if (instance()->GetInfo(path.c_str(), info))
    {
        if (filter != Filesystem::FileType::FILETYPE_MAX_ENUM && info.type != filter)
        {
            lua_pushnil(L);

            return 1;
        }

        const char* type = nullptr;
        if (auto found = Filesystem::fileTypes.ReverseFind(info.type))
            type = *found;
        else
            return luaL_error(L, "Unknown file type.");

        if (lua_istable(L, start))
            lua_pushvalue(L, start);
        else
            lua_createtable(L, 0, 3);

        lua_pushstring(L, type);
        lua_setfield(L, -2, "type");

        luax::PushBoolean(L, info.readOnly);
        lua_setfield(L, -2, "readonly");

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

int Wrap_Filesystem::GetSaveDirectory(lua_State* L)
{
    luax::PushString(L, instance()->GetSaveDirectory());

    return 1;
}

int Wrap_Filesystem::OpenFile(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);
    const char* filemode = luaL_checkstring(L, 2);

    File::Mode mode;
    if (auto found = File::modes.Find(filemode))
        mode = *found;
    else
        return luax::EnumError(L, "file open mode", File::modes, filemode);

    File* self = nullptr;

    try
    {
        self = instance()->OpenFile(filename, mode);
    }
    catch (love::Exception& e)
    {
        return luax::IOError(L, "%s", e.what());
    }

    luax::PushType(L, self);
    self->Release();

    return 1;
}

int Wrap_Filesystem::GetRealDirectory(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);
    std::string directory;

    try
    {
        directory = instance()->GetRealDirectory(filename);
    }
    catch (love::Exception& e)
    {
        return luax::IOError(L, "%s", e.what());
    }

    luax::PushString(L, directory);

    return 1;
}

int Wrap_Filesystem::NewFileData(lua_State* L)
{
    if (lua_gettop(L) == 1)
    {
        int numResults = 0;
        FileData* data = Wrap_Filesystem::GetFileData(L, 1, true, numResults);

        if (data == nullptr)
            return numResults;

        luax::PushType(L, data);
        data->Release();

        return 1;
    }

    size_t length   = 0;
    const void* ptr = nullptr;
    if (luax::IsType(L, 1, Data::type))
    {
        Data* data = Wrap_Data::CheckData(L, 1);

        ptr    = data->GetData();
        length = data->GetSize();
    }
    else if (lua_isstring(L, 1))
        ptr = luaL_checklstring(L, 1, &length);
    else
        return luaL_argerror(L, 1, "string or Data expected");

    const char* filename = luaL_checkstring(L, 2);

    FileData* data = nullptr;
    luax::CatchException(L, [&]() { data = instance()->NewFileData(ptr, length, filename); });

    luax::PushType(L, data);
    data->Release();

    return 1;
}

int Wrap_Filesystem::Read(lua_State* L)
{
    auto type = DataModule::CONTAINER_STRING;
    int start = 1;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        type  = Wrap_DataModule::CheckContainerType(L, 1);
        start = 2;
    }

    const char* filename = luaL_checkstring(L, start + 0);
    int64_t length       = (int64_t)luaL_optinteger(L, start + 1, -1);

    FileData* data = nullptr;

    try
    {
        if (length >= 0)
            data = instance()->Read(filename, length);
        else
            data = instance()->Read(filename);
    }
    catch (love::Exception& e)
    {
        return luax::IOError(L, "%s", e.what());
    }

    if (data == nullptr)
        return luax::IOError(L, "File could not be read.");

    if (type == DataModule::CONTAINER_DATA)
        luax::PushType(L, data);
    else
        lua_pushlstring(L, (const char*)data->GetData(), data->GetSize());

    lua_pushinteger(L, data->GetSize());

    data->Release();

    return 2;
}

int Wrap_Filesystem::Remove(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);

    bool success = instance()->Remove(filename);

    lua_pushboolean(L, success);

    return 1;
}

int Wrap_Filesystem::GetSourceBaseDirectory(lua_State* L)
{
    luax::PushString(L, instance()->GetSourceBaseDirectory());

    return 1;
}

int Wrap_Filesystem::GetWorkingDirectory(lua_State* L)
{
    luax::PushString(L, instance()->GetWorkingDirectory());

    return 1;
}

int Wrap_Filesystem::SetRequirePath(lua_State* L)
{
    std::string element = luaL_checkstring(L, 1);
    auto& requirePath   = instance()->GetRequirePath();

    requirePath.clear();
    std::string path;

    for (char item : element)
    {
        if (item == ';')
        {
            requirePath.push_back(path);
            path.clear();
        }
        else
            path += item;
    }

    return 0;
}

int Wrap_Filesystem::SetIdentity(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    bool append      = luax::OptBoolean(L, 2, false);

    if (!instance()->SetIdentity(name, append))
        return luaL_error(L, "Could not set write directory.");

    return 0;
}

int Wrap_Filesystem::WriteOrAppend(lua_State* L, File::Mode mode)
{
    const char* filename = luaL_checkstring(L, 1);

    const char* input = nullptr;
    size_t length     = 0;

    if (luax::IsType(L, 2, Data::type))
    {
        Data* data = luax::ToType<Data>(L, 2);

        input  = (const char*)data->GetData();
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
            instance()->Append(filename, (const void*)input, length);
        else
            instance()->Write(filename, (const void*)input, length);
    }
    catch (love::Exception& e)
    {
        return luax::IOError(L, "%s", e.what());
    }

    lua_pushboolean(L, true);

    return 1;
}

int Wrap_Filesystem::Write(lua_State* L)
{
    return Wrap_Filesystem::WriteOrAppend(L, File::MODE_WRITE);
}

File* Wrap_Filesystem::GetFile(lua_State* L, int index)
{
    File* file = nullptr;

    if (lua_isstring(L, index))
    {
        std::filesystem::path filename = luaL_checkstring(L, index);
        translatePath(filename);

        file = instance()->OpenFile(filename.c_str(), File::MODE_CLOSED);
    }
    else
    {
        file = Wrap_File::CheckFile(L, index);
        file->Retain();
    }

    return file;
}

int Wrap_Filesystem::Lines(lua_State* L)
{
    if (lua_isstring(L, 1))
    {
        File* file = nullptr;

        const char* filename = lua_tostring(L, 1);

        luax::CatchException(L, [&]() { file = instance()->OpenFile(filename, File::MODE_READ); });

        luax::PushType(L, file);
        file->Release();
    }
    else
        return luaL_argerror(L, 1, "expected filename.");

    lua_pushstring(L, ""); // buffer
    lua_pushstring(L, 0);  // buffer offset
    lua_pushcclosure(L, Wrap_File::Lines_I, 3);

    return 1;
}

/* helper functions */

FileData* Wrap_Filesystem::GetFileData(lua_State* L, int index, bool ioError, int& numResults)
{
    FileData* data = nullptr;
    File* file     = nullptr;

    if (lua_isstring(L, index) || luax::IsType(L, index, File::type))
        file = Wrap_Filesystem::GetFile(L, index);
    else if (luax::IsType(L, index, FileData::type))
    {
        data = Wrap_FileData::CheckFileData(L, index);
        data->Retain();
    }

    if (!data && !file)
    {
        numResults = luaL_argerror(L, index, "filename, File, or FileData expected.");
        return nullptr;
    }
    else if (file && !data)
    {
        try
        {
            data = file->Read();
        }
        catch (love::Exception& e)
        {
            file->Release();

            if (ioError)
                numResults = luax::IOError(L, "%s", e.what());
            else
                numResults = luaL_error(L, "%s", e.what());

            return nullptr;
        }
        file->Release();
    }

    return data;
}

FileData* Wrap_Filesystem::GetFileData(lua_State* L, int index)
{
    int numResults = 0;
    return Wrap_Filesystem::GetFileData(L, index, false, numResults);
}

Data* Wrap_Filesystem::GetData(lua_State* L, int index)
{
    Data* data = nullptr;
    File* file = nullptr;

    if (lua_isstring(L, index) || luax::IsType(L, index, File::type))
    {
        file = Wrap_Filesystem::GetFile(L, index);
    }
    else if (luax::IsType(L, index, Data::type))
    {
        data = Wrap_Data::CheckData(L, index);
        data->Retain();
    }

    if (!data && !file)
    {
        const auto error =
            std::format("filename, File, or Data expected, got {}", luaL_typename(L, index));

        luaL_argerror(L, index, error.c_str());
        return nullptr; // Never reached.
    }

    if (file)
    {
        luax::CatchException(
            L, [&]() { data = file->Read(); }, [&](bool) { file->Release(); });
    }

    return data;
}

bool Wrap_Filesystem::CanGetFile(lua_State* L, int index)
{
    return lua_isstring(L, index) || luax::IsType(L, index, File::type);
}

bool Wrap_Filesystem::CanGetFileData(lua_State* L, int index)
{
    return lua_isstring(L, index) || luax::IsType(L, index, File::type) ||
           luax::IsType(L, index, FileData::type);
}

bool Wrap_Filesystem::CanGetData(lua_State* L, int index)
{
    return lua_isstring(L, index) || luax::IsType(L, index, File::type) ||
           luax::IsType(L, index, Data::type);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "append",                 Wrap_Filesystem::Append                 },
    { "createDirectory",        Wrap_Filesystem::CreateDirectory        },
    { "exists",                 Wrap_Filesystem::Exists                 },
    { "getDirectoryItems",      Wrap_Filesystem::GetDirectoryItems      },
    { "getExecutablePath",      Wrap_Filesystem::GetExecutablePath      },
    { "getIdentity",            Wrap_Filesystem::GetIdentity            },
    { "getInfo",                Wrap_Filesystem::GetInfo                },
    { "getRealDirectory",       Wrap_Filesystem::GetRealDirectory       },
    { "getRequirePath",         Wrap_Filesystem::GetRequirePath         },
    { "getSaveDirectory",       Wrap_Filesystem::GetSaveDirectory       },
    { "getSource",              Wrap_Filesystem::GetSource              },
    { "getSourceBaseDirectory", Wrap_Filesystem::GetSourceBaseDirectory },
    { "getUserDirectory",       Wrap_Filesystem::GetUserDirectory       },
    { "getWorkingDirectory",    Wrap_Filesystem::GetWorkingDirectory    },
    { "init",                   Wrap_Filesystem::Init                   },
    { "isFused",                Wrap_Filesystem::IsFused                },
    { "lines",                  Wrap_Filesystem::Lines                  },
    { "load",                   Wrap_Filesystem::Load                   },
    { "mount",                  Wrap_Filesystem::Mount                  },
    { "mountFullPath",          Wrap_Filesystem::MountFullPath          },
    { "mountCommonPath",        Wrap_Filesystem::MountCommonPath        },
    { "openFile",               Wrap_Filesystem::OpenFile               },
    { "newFileData",            Wrap_Filesystem::NewFileData            },
    { "read",                   Wrap_Filesystem::Read                   },
    { "remove",                 Wrap_Filesystem::Remove                 },
    { "setFused",               Wrap_Filesystem::SetFused               },
    { "setIdentity",            Wrap_Filesystem::SetIdentity            },
    { "setRequirePath",         Wrap_Filesystem::SetRequirePath         },
    { "setSource",              Wrap_Filesystem::SetSource              },
    { "unmount",                Wrap_Filesystem::UnMount                },
    { "unmountFullPath",        Wrap_Filesystem::UnMountFullPath        },
    { "unmountCommonPath",      Wrap_Filesystem::UnMountCommonPath      },
    { "write",                  Wrap_Filesystem::Write                  }
};

static constexpr lua_CFunction types[] =
{
    Wrap_FileData::Register,
    Wrap_File::Register,
    nullptr
};
// clang-format on

int Wrap_Filesystem::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new physfs::Filesystem(); });
    else
        instance->Retain();

    luax::RegisterSearcher(L, Wrap_Filesystem::Loader, 2);

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "filesystem";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = types;

    return luax::RegisterModule(L, wrappedModule);
}
