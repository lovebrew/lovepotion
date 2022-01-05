#include "modules/filesystem/wrap_filesystem.h"

#include <filesystem>

using namespace love;

#define instance() (Module::GetInstance<Filesystem>(Module::M_FILESYSTEM))

static void replaceAll(std::string& str, const std::string& substr, const std::string& replacement)
{
    std::vector<size_t> locations;
    size_t pos    = 0;
    size_t sublen = substr.length();

    while ((pos = str.find(substr, pos)) != std::string::npos)
    {
        locations.push_back(pos);
        pos += sublen;
    }

    for (int i = (int)locations.size() - 1; i >= 0; i--)
        str.replace(locations[i], sublen, replacement);
}

#if defined(__3DS__)
static void translatePath(std::filesystem::path& filepath)
{
    static constexpr std::array<const char*, 3> textures = { ".png", ".jpg", ".jpeg" };
    static constexpr std::array<const char*, 2> fonts    = { ".ttf", ".otf" };

    for (auto extension : textures)
    {
        if (extension == filepath.extension())
            filepath.replace_extension(".t3x");
    }

    for (auto extension : fonts)
    {
        if (extension == filepath.extension())
            filepath.replace_extension(".bcfnt");
    }
}
#endif

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
        return Luax::IOError(L, "%s", e.what());
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

    Luax::CatchException(L, [&]() { instance()->Init(arg0); });

    return 0;
}

int Wrap_Filesystem::Mount(lua_State* L)
{
    std::string archive;

    if (Luax::IsType(L, 1, Data::type))
    {
        Data* data = Wrap_Data::CheckData(L, 1);
        int start  = 2;

        if (Luax::IsType(L, 1, FileData::type) && !lua_isstring(L, 3))
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
        bool append            = Luax::OptBoolean(L, start + 1, false);

        lua_pushboolean(L, instance()->Mount(data, archive.c_str(), mountPoint, append));

        return 1;
    }
    else // Don't check for DroppedFile (as LOVE would) since it's not supported here
        archive = luaL_checkstring(L, 1);

    const char* mountPoint = luaL_checkstring(L, 2);
    bool append            = Luax::OptBoolean(L, 3, false);

    lua_pushboolean(L, instance()->Mount(archive.c_str(), mountPoint, append));

    return 1;
}

int Wrap_Filesystem::UnMount(lua_State* L)
{
    if (Luax::IsType(L, 1, Data::type))
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

int Wrap_Filesystem::GetUserDirectory(lua_State* L)
{
    Luax::PushString(L, instance()->GetUserDirectory());

    return 1;
}

int Wrap_Filesystem::Loader(lua_State* L)
{
    std::string modulename = luaL_checkstring(L, 1);

    for (char& c : modulename)
    {
        if (c == '.')
            c = '/';
    }

    auto* inst = instance();
    for (std::string element : inst->GetRequirePath())
    {
        replaceAll(element, "?", modulename);

        Filesystem::Info info = {};
        if (inst->GetInfo(element.c_str(), info) && info.type != Filesystem::FILETYPE_DIRECTORY)
        {
            lua_pop(L, 1);
            lua_pushstring(L, element.c_str());

            return Wrap_Filesystem::Load(L);
        }
    }

    std::string errstr = "\n    no '%s' in LOVE game directories.";

    lua_pushfstring(L, errstr.c_str(), modulename.c_str());

    return 1;
}

bool SetupWriteDirectory()
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
    Luax::PushString(L, instance()->GetExecutablePath());

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
    lua_pushstring(L, instance()->GetSource());

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
    lua_pushstring(L, instance()->GetIdentity());

    return 1;
}

int Wrap_Filesystem::GetInfo(lua_State* L)
{
    const char* filepath  = luaL_checkstring(L, 1);
    Filesystem::Info info = {};

    int start                   = 2;
    Filesystem::FileType filter = Filesystem::FileType::FILETYPE_MAX_ENUM;

    if (lua_isstring(L, start))
    {
        const char* type = luaL_checkstring(L, start);
        if (!Filesystem::GetConstant(type, filter))
            return Luax::EnumError(L, "file type", Filesystem::GetConstants(filter), type);

        start++;
    }

    std::filesystem::path path = filepath;

#if defined(__3DS__)
    translatePath(path);
#endif

    if (instance()->GetInfo(path.c_str(), info))
    {
        if (filter != Filesystem::FileType::FILETYPE_MAX_ENUM && info.type != filter)
        {
            lua_pushnil(L);

            return 1;
        }

        const char* type = nullptr;
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

int Wrap_Filesystem::GetSaveDirectory(lua_State* L)
{
    Luax::PushString(L, instance()->GetSaveDirectory());

    return 1;
}

int Wrap_Filesystem::NewFile(lua_State* L)
{
    const char* filename = luaL_checkstring(L, 1);

    const char* string = 0;
    File::Mode mode    = File::MODE_CLOSED;

    if (lua_isstring(L, 2))
    {
        string = luaL_checkstring(L, 2);
        if (!File::GetConstant(string, mode))
            return Luax::EnumError(L, "file open mode", File::GetConstants(mode), string);
    }

    File* file = instance()->NewFile(filename);

    if (mode != File::MODE_CLOSED)
    {
        try
        {
            if (!file->Open(mode))
                throw love::Exception("Could not open file.");
        }
        catch (love::Exception& e)
        {
            file->Release();
            return Luax::IOError(L, "%s", e.what());
        }
    }

    Luax::PushType(L, file);
    file->Release();

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
        return Luax::IOError(L, "%s", e.what());
    }

    Luax::PushString(L, directory);

    return 1;
}

int Wrap_Filesystem::NewFileData(lua_State* L)
{
    if (lua_gettop(L) == 1)
    {
        if (lua_isstring(L, 1))
            Luax::ConvertObject(L, 1, "filesystem", "newFile");

        if (Luax::IsType(L, 1, File::type))
        {
            File* file = Wrap_File::CheckFile(L, 1);
            StrongReference<FileData> data;

            try
            {
                data.Set(file->Read(), Acquire::NORETAIN);
            }
            catch (love::Exception& e)
            {
                return Luax::IOError(L, "%s", e.what());
            }

            Luax::PushType(L, data);

            return 1;
        }
        else
            return luaL_argerror(L, 1, "filename or File expected");
    }

    size_t length   = 0;
    const void* ptr = nullptr;
    if (Luax::IsType(L, 1, Data::type))
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
    Luax::CatchException(L, [&]() { data = instance()->NewFileData(ptr, length, filename); });

    Luax::PushType(L, data);
    data->Release();

    return 1;
}

int Wrap_Filesystem::Read(lua_State* L)
{
    love::data::ContainerType type = love::data::CONTAINER_STRING;
    int start                      = 1;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        type  = Wrap_DataModule::CheckContainerType(L, 1);
        start = 2;
    }

    const char* filename = luaL_checkstring(L, start + 0);
    int64_t length       = (int64_t)luaL_optinteger(L, start + 1, File::ALL);

    FileData* data = nullptr;

    try
    {
        data = instance()->Read(filename, length);
    }
    catch (love::Exception& e)
    {
        return Luax::IOError(L, "%s", e.what());
    }

    if (data == nullptr)
        return Luax::IOError(L, "File could not be read.");

    if (type == love::data::CONTAINER_DATA)
        Luax::PushType(L, data);
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
    Luax::PushString(L, instance()->GetSourceBaseDirectory());

    return 1;
}

int Wrap_Filesystem::GetWorkingDirectory(lua_State* L)
{
    lua_pushstring(L, instance()->GetWorkingDirectory());

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
    bool append      = Luax::OptBoolean(L, 2, false);

    if (!instance()->SetIdentity(name, append))
        return luaL_error(L, "Could not set write directory.");

    return 0;
}

int Wrap_Filesystem::WriteOrAppend(lua_State* L, File::Mode mode)
{
    const char* filename = luaL_checkstring(L, 1);

    const char* input = nullptr;
    size_t length     = 0;

    if (Luax::IsType(L, 2, Data::type))
    {
        Data* data = Luax::ToType<Data>(L, 2);

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
        return Luax::IOError(L, "%s", e.what());
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

#if defined(__3DS__)
        translatePath(filename);
#endif

        file = instance()->NewFile(filename.c_str());
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
        File* file   = instance()->NewFile(lua_tostring(L, 1));
        bool success = false;

        Luax::CatchException(L, [&]() { success = file->Open(File::MODE_READ); });

        if (!success)
        {
            file->Release();
            return luaL_error(L, "Could not open file.");
        }

        Luax::PushType(L, file);
        file->Release();
    }
    else
        return luaL_argerror(L, 1, "expected filename.");

    lua_pushstring(L, ""); // buffer
    lua_pushstring(L, 0);  // buffer offset
    lua_pushcclosure(L, Wrap_File::Lines_I, 3);

    return 1;
}
FileData* Wrap_Filesystem::GetFileData(lua_State* L, int index)
{
    FileData* data = nullptr;
    File* file     = nullptr;

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
        Luax::CatchException(
            L, [&]() { data = file->Read(); }, [&](bool) { file->Release(); });
    }

    return data;
}

Data* Wrap_Filesystem::GetData(lua_State* L, int index)
{
    Data* data = nullptr;
    File* file = nullptr;

    if (lua_isstring(L, index) || Luax::IsType(L, index, File::type))
    {
        file = Wrap_Filesystem::GetFile(L, index);
    }
    else if (Luax::IsType(L, index, Data::type))
    {
        data = Wrap_Data::CheckData(L, index);
        data->Retain();
    }

    if (!data && !file)
    {
        luaL_argerror(L, index, "filename, File, or Data expected");
        return nullptr; // Never reached.
    }

    if (file)
    {
        Luax::CatchException(
            L, [&]() { data = file->Read(); }, [&](bool) { file->Release(); });
    }

    return data;
}

bool Wrap_Filesystem::CanGetData(lua_State* L, int index)
{
    return lua_isstring(L, index) || Luax::IsType(L, index, File::type) ||
           Luax::IsType(L, index, Data::type);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "append",                 Wrap_Filesystem::Append                 },
    { "createDirectory",        Wrap_Filesystem::CreateDirectory        },
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
    { "newFile",                Wrap_Filesystem::NewFile                },
    { "newFileData",            Wrap_Filesystem::NewFileData            },
    { "read",                   Wrap_Filesystem::Read                   },
    { "remove",                 Wrap_Filesystem::Remove                 },
    { "setFused",               Wrap_Filesystem::SetFused               },
    { "setIdentity",            Wrap_Filesystem::SetIdentity            },
    { "setRequirePath",         Wrap_Filesystem::SetRequirePath         },
    { "setSource",              Wrap_Filesystem::SetSource              },
    { "unmount",                Wrap_Filesystem::UnMount                },
    { "write",                  Wrap_Filesystem::Write                  },
    { 0,                        0                                       }
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
    Filesystem* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Filesystem(); });
    else
        instance->Retain();

    Luax::RegisterSearcher(L, Loader, 2);

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "filesystem";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = types;

    return Luax::RegisterModule(L, wrappedModule);
}
