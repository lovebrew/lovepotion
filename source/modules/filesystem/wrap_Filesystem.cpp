#include "modules/filesystem/wrap_Filesystem.hpp"

#include "modules/data/wrap_Data.hpp"
#include "modules/data/wrap_DataModule.hpp"
#include "modules/filesystem/physfs/Filesystem.hpp"

#include "modules/filesystem/wrap_File.hpp"
#include "modules/filesystem/wrap_FileData.hpp"
#include "modules/filesystem/wrap_NativeFile.hpp"

#include "common/Console.hpp"
#include "common/config.hpp"

using namespace love;

#define instance() (Module::getInstance<Filesystem>(Module::M_FILESYSTEM))

#include <algorithm>
#include <string_view>

static void replaceAll(std::string& inout, std::string_view what, std::string_view with)
{
    std::string::size_type pos = 0;

    while ((pos = inout.find(what, pos)) != std::string::npos)
    {
        inout.replace(pos, what.length(), with);
        pos += with.length();
    }
}

static std::filesystem::path translatePath(const std::filesystem::path& input)
{
    if (!Console::is(Console::CTR))
        return input;

    static constexpr auto textures = { ".png", ".jpg", ".jpeg" };
    static constexpr auto fonts    = { ".ttf", ".otf" };

    auto path = std::filesystem::path(input);

    if (std::find(textures.begin(), textures.end(), input.extension()) != textures.end())
        return path.replace_extension(".t3x");
    else if (std::find(fonts.begin(), fonts.end(), input.extension()) != fonts.end())
        return path.replace_extension(".bcfnt");

    return path;
}

int Wrap_Filesystem::init(lua_State* L)
{
    const char* arg0 = luaL_checkstring(L, 1);

    luax_catchexcept(L, [&] { instance()->init(arg0); });

    return 0;
}

int Wrap_Filesystem::setAndroidSaveExternal(lua_State* L)
{
    LOVE_UNUSED(L);
    return 0;
}

int Wrap_Filesystem::setFused(lua_State* L)
{
    bool fused = luax_toboolean(L, 1);

    instance()->setFused(fused);

    return 0;
}

int Wrap_Filesystem::isFused(lua_State* L)
{
    luax_pushboolean(L, instance()->isFused());

    return 1;
}

int Wrap_Filesystem::setIdentity(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    bool append      = luax_optboolean(L, 2, false);

    if (!instance()->setIdentity(name, append))
        return luaL_error(L, "Could not set write directory.");

    return 0;
}

int Wrap_Filesystem::getIdentity(lua_State* L)
{
    luax_pushstring(L, instance()->getIdentity());

    return 1;
}

int Wrap_Filesystem::setSource(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);

    if (!instance()->setSource(name))
        return luaL_error(L, "Could not set source directory to '%s'.", name);

    return 0;
}

int Wrap_Filesystem::getSource(lua_State* L)
{
    luax_pushstring(L, instance()->getSource());

    return 1;
}

int Wrap_Filesystem::mount(lua_State* L)
{
    std::string archive {};
    if (luax_istype(L, 1, Data::type))
    {
        auto* data = luax_checkdata(L, 1);
        int start  = 2;

        if (luax_istype(L, 1, FileData::type) && !lua_isstring(L, 3))
        {
            auto* fileData = luax_checkfiledata(L, 1);
            archive        = fileData->getFilename();
            start          = 2;
        }
        else
        {
            archive = luaL_checkstring(L, 2);
            start   = 3;
        }

        const char* mountPoint = luaL_checkstring(L, start + 0);
        bool append            = luax_optboolean(L, start + 1, false);

        luax_pushboolean(L, instance()->mount(data, archive.c_str(), mountPoint, append));
        return 1;
    }
    else
        archive = luax_checkstring(L, 1);

    const char* mountPoint = luaL_checkstring(L, 2);
    bool append            = luax_optboolean(L, 3, false);

    luax_pushboolean(L, instance()->mount(archive.c_str(), mountPoint, append));
    return 1;
}

int Wrap_Filesystem::mountFullPath(lua_State* L)
{
    const char* fullpath   = luaL_checkstring(L, 1);
    const char* mountPoint = luaL_checkstring(L, 2);

    auto permissions = Filesystem::MOUNT_PERMISSIONS_READ;

    if (!lua_isnoneornil(L, 3))
    {
        const char* type = luaL_checkstring(L, 3);
        if (!Filesystem::getConstant(type, permissions))
            return luax_enumerror(L, "mount permissions", Filesystem::MountPermissionsType, type);
    }

    bool append = luax_optboolean(L, 4, false);
    luax_pushboolean(L, instance()->mountFullPath(fullpath, mountPoint, permissions, append));

    return 1;
}

int Wrap_Filesystem::mountCommonPath(lua_State* L)
{
    const char* commonPathStr = luaL_checkstring(L, 1);
    auto commonPath           = Filesystem::COMMONPATH_MAX_ENUM;

    if (!Filesystem::getConstant(commonPathStr, commonPath))
        return luax_enumerror(L, "common path", Filesystem::CommonPaths, commonPathStr);

    const char* mountPoint = luaL_checkstring(L, 2);

    auto permissions = Filesystem::MOUNT_PERMISSIONS_READ;
    if (!lua_isnoneornil(L, 3))
    {
        const char* type = luaL_checkstring(L, 3);
        if (!Filesystem::getConstant(type, permissions))
            return luax_enumerror(L, "mount permissions", Filesystem::MountPermissionsType, type);
    }

    bool append = luax_optboolean(L, 4, false);

    luax_pushboolean(L, instance()->mountCommonPath(commonPath, mountPoint, permissions, append));

    return 1;
}

int Wrap_Filesystem::unmount(lua_State* L)
{
    if (luax_istype(L, 1, Data::type))
    {
        Data* data = luax_checkdata(L, 1);
        luax_pushboolean(L, instance()->unmount(data));
    }
    else
    {
        const char* archive = luaL_checkstring(L, 1);
        luax_pushboolean(L, instance()->unmount(archive));
    }

    return 1;
}

int Wrap_Filesystem::unmountFullPath(lua_State* L)
{
    const char* fullpath = luaL_checkstring(L, 1);

    luax_pushboolean(L, instance()->unmountFullPath(fullpath));

    return 1;
}

int Wrap_Filesystem::unmountCommonPath(lua_State* L)
{
    const char* commonPathStr = luaL_checkstring(L, 1);
    auto commonPath           = Filesystem::COMMONPATH_MAX_ENUM;

    if (!Filesystem::getConstant(commonPathStr, commonPath))
        return luax_enumerror(L, "common path", Filesystem::CommonPaths, commonPathStr);

    luax_pushboolean(L, instance()->unmount(commonPath));

    return 1;
}

int Wrap_Filesystem::openFile(lua_State* L)
{
    const char* filename   = luaL_checkstring(L, 1);
    const char* modeString = luaL_checkstring(L, 2);

    File::Mode mode = File::MODE_CLOSED;
    if (!File::getConstant(modeString, mode))
        return luax_enumerror(L, "file open mode", File::OpenModes, modeString);

    FileBase* file = nullptr;

    try
    {
        file = instance()->openFile(filename, mode);
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    luax_pushtype(L, file);
    file->release();

    return 1;
}

int Wrap_Filesystem::openNativeFile(lua_State* L)
{
    const char* filename   = luaL_checkstring(L, 1);
    const char* modeString = luaL_checkstring(L, 2);

    File::Mode mode = File::MODE_CLOSED;
    if (!File::getConstant(modeString, mode))
        return luax_enumerror(L, "file open mode", File::OpenModes, modeString);

    FileBase* file = nullptr;

    try
    {
        file = instance()->openNativeFile(filename, mode);
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    luax_pushtype(L, file);
    file->release();

    return 1;
}

int Wrap_Filesystem::getFullCommonPath(lua_State* L)
{
    const char* commonPathStr = luaL_checkstring(L, 1);
    auto commonPath           = Filesystem::COMMONPATH_MAX_ENUM;

    if (!Filesystem::getConstant(commonPathStr, commonPath))
        return luax_enumerror(L, "common path", Filesystem::CommonPaths, commonPathStr);

    luax_pushstring(L, instance()->getFullCommonPath(commonPath));

    return 1;
}

int Wrap_Filesystem::getWorkingDirectory(lua_State* L)
{
    luax_pushstring(L, instance()->getWorkingDirectory());

    return 1;
}

int Wrap_Filesystem::getUserDirectory(lua_State* L)
{
    luax_pushstring(L, instance()->getUserDirectory());

    return 1;
}

int Wrap_Filesystem::getAppdataDirectory(lua_State* L)
{
    luax_pushstring(L, instance()->getAppdataDirectory());

    return 1;
}

int Wrap_Filesystem::getSaveDirectory(lua_State* L)
{
    luax_pushstring(L, instance()->getSaveDirectory());

    return 1;
}

int Wrap_Filesystem::getSourceBaseDirectory(lua_State* L)
{
    luax_pushstring(L, instance()->getSourceBaseDirectory());

    return 1;
}

int Wrap_Filesystem::getRealDirectory(lua_State* L)
{
    const char* filepath = luaL_checkstring(L, 1);
    std::string directory {};

    try
    {
        directory = instance()->getRealDirectory(filepath);
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    luax_pushstring(L, directory);

    return 1;
}

int Wrap_Filesystem::getExecutablePath(lua_State* L)
{
    luax_pushstring(L, instance()->getExecutablePath());

    return 1;
}

int Wrap_Filesystem::createDirectory(lua_State* L)
{
    const char* path = luaL_checkstring(L, 1);
    luax_pushboolean(L, instance()->createDirectory(path));

    return 1;
}

int Wrap_Filesystem::remove(lua_State* L)
{
    auto path = translatePath(luaL_checkstring(L, 1));
    luax_pushboolean(L, instance()->remove(path.c_str()));

    return 1;
}

int Wrap_Filesystem::read(lua_State* L)
{
    auto containerType = data::CONTAINER_STRING;
    int start          = 1;

    if (lua_type(L, 2) == LUA_TSTRING)
    {
        containerType = luax_checkcontainertype(L, 1);
        start         = 2;
    }

    auto filename  = translatePath(luaL_checkstring(L, start + 0));
    int64_t length = luaL_optinteger(L, start + 1, -1);

    FileData* data = nullptr;

    try
    {
        if (length >= 0)
            data = instance()->read(filename.c_str(), length);
        else
            data = instance()->read(filename.c_str());
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    if (data == nullptr)
        return luax_ioerror(L, "File could not be read.");

    if (containerType == data::CONTAINER_DATA)
        luax_pushtype(L, data);
    else
        lua_pushlstring(L, (const char*)data->getData(), data->getSize());

    lua_pushinteger(L, data->getSize());
    data->release();

    return 2;
}

static int write_or_append(lua_State* L, File::Mode mode)
{
    const char* filename = luaL_checkstring(L, 1);

    const char* input = nullptr;
    size_t length     = 0;

    if (luax_istype(L, 2, Data::type))
    {
        auto* data = luax_totype<Data>(L, 2);
        input      = (const char*)data->getData();
        length     = data->getSize();
    }
    else if (lua_isstring(L, 2))
        input = lua_tolstring(L, 2, &length);
    else
        return luaL_argerror(L, 2, "string or Data expected");

    length = luaL_optinteger(L, 3, length);

    try
    {
        if (mode == File::MODE_APPEND)
            instance()->append(filename, (const void*)input, length);
        else
            instance()->write(filename, (const void*)input, length);
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    luax_pushboolean(L, true);

    return 1;
}

int Wrap_Filesystem::write(lua_State* L)
{
    return write_or_append(L, File::MODE_WRITE);
}

int Wrap_Filesystem::append(lua_State* L)
{
    return write_or_append(L, File::MODE_APPEND);
}

int Wrap_Filesystem::getDirectoryItems(lua_State* L)
{
    const char* directory = luaL_checkstring(L, 1);
    std::vector<std::string> items {};

    instance()->getDirectoryItems(directory, items);

    lua_createtable(L, items.size(), 0);

    for (size_t i = 0; i < items.size(); i++)
    {
        lua_pushstring(L, items[i].c_str());
        lua_rawseti(L, -2, i + 1);
    }

    return 1;
}

int Wrap_Filesystem::lines(lua_State* L)
{
    if (lua_isstring(L, 1))
    {
        FileBase* file = nullptr;

        const char* filename = luaL_checkstring(L, 1);
        luax_catchexcept(L, [&] { file = instance()->openFile(filename, File::MODE_READ); });

        luax_pushtype(L, file);
        file->release();
    }
    else
        return luaL_argerror(L, 1, "expected filename.");

    lua_pushstring(L, "");
    lua_pushstring(L, 0);
    lua_pushcclosure(L, Wrap_File::lines_i, 3);

    return 1;
}

int Wrap_Filesystem::exists(lua_State* L)
{
    auto filename = translatePath(luaL_checkstring(L, 1));
    luax_pushboolean(L, instance()->exists(filename.c_str()));

    return 1;
}

int Wrap_Filesystem::load(lua_State* L)
{
    std::string filename      = luaL_checkstring(L, 1);
    Filesystem::LoadMode mode = Filesystem::LOADMODE_ANY;

    if (!lua_isnoneornil(L, 2))
    {
        const char* modeStr = luaL_checkstring(L, 2);
        if (!Filesystem::getConstant(modeStr, mode))
            return luax_enumerror(L, "load mode", Filesystem::LoadModes, modeStr);
    }

    Data* data = nullptr;

    try
    {
        data = instance()->read(filename.c_str());
    }
    catch (love::Exception& e)
    {
        return luax_ioerror(L, "%s", e.what());
    }

    int status = 0;

    // clang-format off
#if (LUA_VERSION_NUM > 501) || defined(LUA_JITLIBNAME)
    const char* modeStr = nullptr;
    Filesystem::getConstant(mode, modeStr);

    status = luaL_loadbufferx(L, (const char*)data->getData(), data->getSize(), filename.c_str(), modeStr);
#else
    if (mode == Filesystem::LOADMODE_ANY)
        status = luaL_loadbuffer(L, (const char*)data->getData(), data->getSize(), filename.c_str());
    else
    {
        data->release();
        return luaL_error(L, "Only \"bt\" is supported on this Lua interpreter.\n");
    }
    // clang-format on
#endif

    data->release();

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

int Wrap_Filesystem::getInfo(lua_State* L)
{
    auto filepath = translatePath(luaL_checkstring(L, 1));
    Filesystem::Info info {};

    int start       = 2;
    auto filterType = Filesystem::FILETYPE_MAX_ENUM;

    if (lua_isstring(L, start))
    {
        const char* typeString = luaL_checkstring(L, start);
        if (!Filesystem::getConstant(typeString, filterType))
            return luax_enumerror(L, "file type", Filesystem::FileTypes, typeString);

        start++;
    }

    if (instance()->getInfo(filepath.c_str(), info))
    {
        if (filterType != Filesystem::FILETYPE_MAX_ENUM && info.type != filterType)
        {
            lua_pushnil(L);
            return 1;
        }

        std::string_view type {};
        if (!Filesystem::getConstant(info.type, type))
            return luaL_error(L, "Unknown file type.");

        if (lua_istable(L, start))
            lua_pushvalue(L, start);
        else
            lua_createtable(L, 0, 3);

        luax_pushstring(L, type);
        lua_setfield(L, -2, "type");

        luax_pushboolean(L, info.readonly);
        lua_setfield(L, -2, "readonly");

        info.size = std::min(info.size, File::MAX_FILE_SIZE);
        if (info.size >= 0)
        {
            lua_pushnumber(L, (lua_Number)info.size);
            lua_setfield(L, -2, "size");
        }

        info.modtime = std::min(info.modtime, File::MAX_MODTIME);
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

int Wrap_Filesystem::setSymlinksEnabled(lua_State* L)
{
    bool enable = luax_checkboolean(L, 1);
    instance()->setSymlinksEnabled(enable);

    return 0;
}

int Wrap_Filesystem::areSymlinksEnabled(lua_State* L)
{
    luax_pushboolean(L, instance()->areSymlinksEnabled());

    return 1;
}

int Wrap_Filesystem::newFileData(lua_State* L)
{
    if (lua_gettop(L) == 1)
    {
        int results = 0;
        auto* data  = luax_getfiledata(L, 1, true, results);

        if (data == nullptr)
            return results;

        luax_pushtype(L, data);
        data->release();

        return 1;
    }

    size_t length       = 0;
    const void* pointer = nullptr;

    if (luax_istype(L, 1, Data::type))
    {
        auto* data = luax_checkdata(L, 1);
        pointer    = data->getData();
        length     = data->getSize();
    }
    else if (lua_isstring(L, 1))
        pointer = luaL_checklstring(L, 1, &length);
    else
        return luaL_argerror(L, 1, "string or Data expected");

    const char* filename = luaL_checkstring(L, 2);

    FileData* data = nullptr;
    luax_catchexcept(L, [&] { data = instance()->newFileData(pointer, length, filename); });

    luax_pushtype(L, data);
    data->release();

    return 1;
}

int Wrap_Filesystem::getRequirePath(lua_State* L)
{
    std::string path;
    bool separator = false;

    for (const auto& element : instance()->getRequirePath())
    {
        if (separator)
            path += ";";
        else
            separator = true;

        path += element;
    }

    luax_pushstring(L, path);

    return 1;
}

int Wrap_Filesystem::setRequirePath(lua_State* L)
{
    std::string element = luax_checkstring(L, 1);
    auto& requirePath   = instance()->getRequirePath();

    requirePath.clear();

    size_t startPos = 0;
    size_t endPos   = element.find(';');

    while (endPos != std::string::npos)
    {
        requirePath.push_back(element.substr(startPos, endPos - startPos));
        startPos = endPos + 1;
        endPos   = element.find(';', startPos);
    }

    requirePath.push_back(element.substr(startPos));

    return 0;
}

int Wrap_Filesystem::getCRequirePath(lua_State* L)
{
    std::string path;
    bool separator = false;

    for (const auto& element : instance()->getCRequirePath())
    {
        if (separator)
            path += ";";
        else
            separator = true;

        path += element;
    }

    luax_pushstring(L, path);

    return 1;
}

int Wrap_Filesystem::setCRequirePath(lua_State* L)
{
    std::string element = luax_checkstring(L, 1);
    auto& requirePath   = instance()->getCRequirePath();

    requirePath.clear();

    size_t startPos = 0;
    size_t endPos   = element.find(';');

    while (endPos != std::string::npos)
    {
        requirePath.push_back(element.substr(startPos, endPos - startPos));
        startPos = endPos + 1;
        endPos   = element.find(';', startPos);
    }

    requirePath.push_back(element.substr(startPos));

    return 0;
}

static int loader(lua_State* L)
{
    std::string moduleName = luax_checkstring(L, 1);

    for (char& c : moduleName)
    {
        if (c == '.')
            c = '/';
    }

    auto* instance = instance();

    for (std::string element : instance->getRequirePath())
    {
        replaceAll(element, "?", moduleName);

        Filesystem::Info info {};
        if (instance->getInfo(element.c_str(), info) && info.type != Filesystem::FILETYPE_DIRECTORY)
        {
            lua_pop(L, 1);
            lua_pushstring(L, element.c_str());

            return Wrap_Filesystem::load(L);
        }
    }

    lua_pushfstring(L, E_NO_FILE_IN_LOVE_DIRS, moduleName.c_str());
    return 1;
}

namespace love
{
    FileBase* luax_getfile(lua_State* L, int index)
    {
        FileBase* result = nullptr;

        if (lua_isstring(L, index))
        {
            auto filename = translatePath(luaL_checkstring(L, index));

            try
            {
                result = instance()->openFile(filename.c_str(), File::MODE_CLOSED);
            }
            catch (love::Exception& e)
            {
                luax_ioerror(L, "%s", e.what());
            }
        }
        else
        {
            result = luax_checkfile(L, index);
            result->retain();
        }

        return result;
    }

    FileData* luax_getfiledata(lua_State* L, int index, bool ioerror, int& results)
    {
        FileData* data = nullptr;
        FileBase* file = nullptr;
        results        = 0;

        if (lua_isstring(L, index) || luax_istype(L, index, File::type))
            file = luax_getfile(L, index);
        else if (luax_istype(L, index, FileData::type))
        {
            data = luax_checkfiledata(L, index);
            data->retain();
        }

        if (!data && !file)
        {
            results = luaL_argerror(L, index, "filename, File, or FileData expected");
            return nullptr;
        }
        else if (file && !data)
        {
            try
            {
                data = file->read();
            }
            catch (love::Exception& e)
            {
                file->release();

                if (ioerror)
                    results = luax_ioerror(L, "%s", e.what());
                else
                    results = luaL_error(L, "%s", e.what());

                return nullptr;
            }

            file->release();
        }

        return data;
    }

    FileData* luax_getfiledata(lua_State* L, int index)
    {
        int results = 0;
        return luax_getfiledata(L, index, false, results);
    }

    Data* luax_getdata(lua_State* L, int index)
    {
        Data* data     = nullptr;
        FileBase* file = nullptr;

        if (lua_isstring(L, index) || luax_istype(L, index, File::type))
            file = luax_getfile(L, index);
        else if (luax_istype(L, index, Data::type))
        {
            data = luax_checkdata(L, index);
            data->retain();
        }

        if (!data && !file)
        {
            luaL_argerror(L, index, "filename, File, or Data expected");
            return nullptr;
        }

        if (file)
        {
            // clang-format off
            luax_catchexcept(L,
                [&] { data = file->read();   },
                [&](bool) { file->release(); }
            );
            // clang-format on
        }

        return data;
    }

    bool luax_cangetfile(lua_State* L, int index)
    {
        return lua_isstring(L, index) || luax_istype(L, index, File::type);
    }

    bool luax_cangetfiledata(lua_State* L, int index)
    {
        return luax_cangetfile(L, index) || luax_istype(L, index, FileData::type);
    }

    bool luax_cangetdata(lua_State* L, int index)
    {
        return luax_cangetfile(L, index) || luax_istype(L, index, Data::type);
    }
} // namespace love

// clang-format off
static constexpr luaL_Reg functions[]
{
    { "append",                  Wrap_Filesystem::append                 },
    { "exists",                  Wrap_Filesystem::exists                 },
    { "getAppdataDirectory",     Wrap_Filesystem::getAppdataDirectory    },
    { "getExecutablePath",       Wrap_Filesystem::getExecutablePath      },
    { "getIdentity",             Wrap_Filesystem::getIdentity            },
    { "getRealDirectory",        Wrap_Filesystem::getRealDirectory       },
    { "getSaveDirectory",        Wrap_Filesystem::getSaveDirectory       },
    { "getSource",               Wrap_Filesystem::getSource              },
    { "getSourceBaseDirectory",  Wrap_Filesystem::getSourceBaseDirectory },
    { "getUserDirectory",        Wrap_Filesystem::getUserDirectory       },
    { "getWorkingDirectory",     Wrap_Filesystem::getWorkingDirectory    },
    { "init",                    Wrap_Filesystem::init                   },
    { "isFused",                 Wrap_Filesystem::isFused                },
    { "load",                    Wrap_Filesystem::load                   },
    { "setFused",                Wrap_Filesystem::setFused               },
    { "setIdentity",             Wrap_Filesystem::setIdentity            },
    { "setSource",               Wrap_Filesystem::setSource              },
    { "write",                   Wrap_Filesystem::write                  },
    { "lines",                   Wrap_Filesystem::lines                  },
    { "setRequirePath",          Wrap_Filesystem::setRequirePath         },
    { "getRequirePath",          Wrap_Filesystem::getRequirePath         },
    { "openFile",                Wrap_Filesystem::openFile               },
    { "openNativeFile",          Wrap_Filesystem::openNativeFile         },
    { "newFileData",             Wrap_Filesystem::newFileData            },
    { "getDirectoryItems",       Wrap_Filesystem::getDirectoryItems      },
    { "createDirectory",         Wrap_Filesystem::createDirectory        },
    { "remove",                  Wrap_Filesystem::remove                 },
    { "read",                    Wrap_Filesystem::read                   },
    { "mount",                   Wrap_Filesystem::mount                  },
    { "mountFullPath",           Wrap_Filesystem::mountFullPath          },
    { "mountCommonPath",         Wrap_Filesystem::mountCommonPath        },
    { "unmount",                 Wrap_Filesystem::unmount                },
    { "unmountFullPath",         Wrap_Filesystem::unmountFullPath        },
    { "unmountCommonPath",       Wrap_Filesystem::unmountCommonPath      },
    { "getFullCommonPath",       Wrap_Filesystem::getFullCommonPath      },
    { "getInfo",                 Wrap_Filesystem::getInfo                },
    { "setSymlinksEnabled",      Wrap_Filesystem::setSymlinksEnabled     },
    { "areSymlinksEnabled",      Wrap_Filesystem::areSymlinksEnabled     },
    { "getCRequirePath",         Wrap_Filesystem::getCRequirePath        },
    { "setCRequirePath",         Wrap_Filesystem::setCRequirePath        },
    { "_setAndroidSaveExternal", Wrap_Filesystem::setAndroidSaveExternal }
};

static constexpr lua_CFunction types[] =
{
    love::open_file,
    love::open_filedata,
    love::open_nativefile
};
// clang-format on

int Wrap_Filesystem::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&] { instance = new Filesystem(); });
    else
        instance->retain();

    luax_register_searcher(L, loader, 2);

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "filesystem";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax_register_module(L, module);
}
