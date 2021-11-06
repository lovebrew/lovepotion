#include "modules/filesystem/filesystem.h"

#include <physfs.h>

#include <limits.h>
#include <sys/stat.h>

#include <unistd.h>

#if defined(__3DS__)
    #include <3ds.h>
#elif defined(__SWITCH__)
    #include <switch.h>
#endif

#include <filesystem>

#define LOVE_APPDATA_FOLDER ""
#define LOVE_APPDATA_PREFIX ""
#define LOVE_PATH_SEPARATOR "/"
#define LOVE_MAX_PATH       PATH_MAX

using namespace love;

love::Type Filesystem::type("Filesystem", &Module::type);

namespace
{
    std::string normalize(const std::string& input)
    {
        std::string out;
        bool seenSep = false, isSep = false;

        for (size_t i = 0; i < input.size(); ++i)
        {
            isSep = (input[i] == LOVE_PATH_SEPARATOR[0]);

            if (!isSep || !seenSep)
                out += input[i];

            seenSep = isSep;
        }

        return out;
    }
} // namespace

Filesystem::Filesystem() : fused(false), fusedSet(false)
{
    requirePath  = { "?.lua", "?/init.lua" };
    cRequirePath = { "??" };
}

void Filesystem::Init(const char* arg0)
{
    if (!PHYSFS_init(arg0))
        throw love::Exception("Failed to initialize filesystem: %s",
                              PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));

    this->exePath = arg0;

    Result result = romfsInit();
    if (R_FAILED(result))
        throw love::Exception("Failed to initialize romfs: %x", result);

    if (!PHYSFS_mount("romfs:/", "internal", true))
        throw love::Exception("Could not mount romFS");

    /* Symlinks don't exist on 3DS/Switch */
    this->SetSymLinksEnabled(false);
}

Filesystem::~Filesystem()
{
    if (PHYSFS_isInit())
        PHYSFS_deinit();
}

void Filesystem::Append(const char* filename, const void* data, int64_t size)
{
    File file(filename);
    file.Open(File::MODE_APPEND);

    if (!file.Write(data, size))
        throw love::Exception("Data could not be written.");
}

std::string Filesystem::GetRealDirectory(const char* filename) const
{
    if (!PHYSFS_isInit())
        throw love::Exception("PhysFS is not initialized.");

    const char* dir = PHYSFS_getRealDir(filename);

    if (dir == nullptr)
        throw love::Exception("File does not exist on disk.");

    return std::string(dir);
}

bool Filesystem::CreateDirectory(const char* name)
{
    if (!PHYSFS_isInit())
        return false;

    if (PHYSFS_getWriteDir() == 0 && !SetupWriteDirectory())
        return false;

    if (!PHYSFS_mkdir(name))
        return false;

    return true;
}

void Filesystem::GetDirectoryItems(const char* directory, std::vector<std::string>& items)
{
    if (!PHYSFS_isInit())
        return;

    char** results = PHYSFS_enumerateFiles(directory);

    if (results == nullptr)
        return;

    for (char** item = results; *item != 0; item++)
        items.push_back(*item);

    PHYSFS_freeList(results);
}

bool Filesystem::SetupWriteDirectory()
{
    if (!PHYSFS_isInit())
        return false;

    if (this->identity.empty() || this->fullSavePath.empty() || this->relativeSavePath.empty())
        return false;

    std::string tmpWriteDirectory  = this->fullSavePath;
    std::string tmpCreateDirectory = this->fullSavePath;

    if (this->fullSavePath.find(this->GetUserDirectory()) == 0)
    {
        tmpWriteDirectory  = this->GetUserDirectory();
        tmpCreateDirectory = this->fullSavePath.substr(this->GetUserDirectory().length());

        // Strip leading '/' characters from the path we want to create.
        size_t startPosition = tmpCreateDirectory.find_first_not_of('/');
        if (startPosition != std::string::npos)
            tmpCreateDirectory = tmpCreateDirectory.substr(startPosition);
    }

    if (!PHYSFS_setWriteDir(tmpWriteDirectory.c_str()))
        return false;

    if (!this->CreateDirectory(tmpCreateDirectory.c_str()))
    {
        PHYSFS_setWriteDir(nullptr);
        return false;
    }

    if (!PHYSFS_setWriteDir(this->fullSavePath.c_str()))
        return false;

    if (!PHYSFS_mount(this->fullSavePath.c_str(), nullptr, 0))
    {
        // Clear the write directory in case of error.
        PHYSFS_setWriteDir(nullptr);
        return false;
    }

    return true;
}

const char* Filesystem::GetIdentity()
{
    return this->identity.c_str();
}

bool Filesystem::GetInfo(const char* filepath, Filesystem::Info& info) const
{
    if (!PHYSFS_isInit())
        return false;

    PHYSFS_Stat stat = {};

    if (!PHYSFS_stat(filepath, &stat))
        return false;

    info.modtime = std::min<int64_t>(stat.modtime, MAX_STAMP);
    info.size    = std::min<int64_t>(stat.filesize, MAX_STAMP);

    if (stat.filetype == PHYSFS_FILETYPE_REGULAR)
        info.type = FILETYPE_FILE;
    else if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY)
        info.type = FILETYPE_DIRECTORY;
    else if (stat.filetype == PHYSFS_FILETYPE_SYMLINK)
        info.type = FILETYPE_SYMLINK;
    else
        info.type = FILETYPE_OTHER;

    return true;
}

std::string Filesystem::GetSaveDirectory()
{
    return this->fullSavePath;
}

File* Filesystem::NewFile(const char* filename)
{
    return new File(filename);
}

FileData* Filesystem::NewFileData(const void* data, size_t size, const char* filename)
{
    FileData* fileData = new FileData(size, filename);
    memcpy(fileData->GetData(), data, size);

    return fileData;
}

FileData* Filesystem::Read(const char* filename, int64_t size)
{
    File file(filename);

    file.Open(File::MODE_READ);

    return file.Read(size);
}

bool Filesystem::Remove(const char* filename)
{
    if (!PHYSFS_isInit())
        return false;

    if (PHYSFS_getWriteDir() == 0 && !SetupWriteDirectory())
        return false;

    if (!PHYSFS_delete(filename))
        return false;

    return true;
}

std::string Filesystem::GetExecutablePath() const
{
    return this->exePath;
}

void Filesystem::SetFused(bool fused)
{
    if (this->fusedSet)
        return;

    this->fused    = fused;
    this->fusedSet = true;
}

const char* Filesystem::GetWorkingDirectory()
{
    if (this->cwd.empty())
    {
        char* cwd_char = new char[LOVE_MAX_PATH];

        if (getcwd(cwd_char, LOVE_MAX_PATH))
            this->cwd = cwd_char;

        delete[] cwd_char;
    }

    return this->cwd.c_str();
}

void Filesystem::SetSymLinksEnabled(bool enable)
{
    if (!PHYSFS_isInit())
        return;

    PHYSFS_permitSymbolicLinks(enable);
}

bool Filesystem::IsFused() const
{
    if (!this->fusedSet)
        return false;

    return this->fused;
}

const char* Filesystem::GetSource() const
{
    return this->gameSource.c_str();
}

bool Filesystem::SetSource(const char* source)
{
    if (!PHYSFS_isInit())
        return false;

    if (!this->gameSource.empty())
        return false;

    std::string searchPath = source;
    if (!PHYSFS_mount(searchPath.c_str(), nullptr, 1))
        return false;

    this->gameSource = searchPath;

    return true;
}

std::string Filesystem::GetSourceBaseDirectory() const
{
    size_t length = this->gameSource.length();

    if (length == 0)
        return "";

    size_t end = this->gameSource.find_last_of("/", length - 2);

    if (end == std::string::npos)
        return "";

    if (end == 0)
        end = 1;

    return this->gameSource.substr(0, end);
}

bool Filesystem::Mount(const char* archive, const char* mountpoint, bool appendToPath)
{
    if (!PHYSFS_isInit() || !archive)
        return false;

    std::string realPath;
    std::string sourceBase = this->GetSourceBaseDirectory();

    auto iterator =
        std::find(this->allowedMountPaths.begin(), this->allowedMountPaths.end(), archive);

    if (iterator != this->allowedMountPaths.end())
        realPath = *iterator;
    else if (this->fused && sourceBase.compare(archive) == 0)
    {
        /*
        ** Special case: if the game is fused and the archive is the source's
        ** base directory, mount it even though it's outside of the save dir.
        */
        realPath = sourceBase;
    }
    else
    {
        if (strlen(archive) == 0 || strstr(archive, "..") || strcmp(archive, "/") == 0)
            return false;

        const char* realDirectory = PHYSFS_getRealDir(archive);

        if (!realDirectory)
            return false;

        realPath = realDirectory;

        if (realPath.find(this->gameSource) == 0)
            return false;

        realPath += LOVE_PATH_SEPARATOR;
        realPath += archive;
    }

    if (realPath.length() == 0)
        return false;

    return PHYSFS_mount(realPath.c_str(), mountpoint, appendToPath) != 0;
}

bool Filesystem::Mount(Data* data, const char* archive, const char* mountpoint, bool appendToPath)
{
    if (!PHYSFS_isInit())
        return false;

    if (PHYSFS_mountMemory(data->GetData(), data->GetSize(), nullptr, archive, mountpoint,
                           appendToPath) != 0)
    {
        this->mountedData[archive] = data;
        return true;
    }

    return false;
}

bool Filesystem::UnMount(const char* archive)
{
    if (!PHYSFS_isInit() || !archive)
        return false;

    auto dataIterator = this->mountedData.find(archive);

    if (dataIterator != this->mountedData.end() && PHYSFS_unmount(archive) != 0)
    {
        this->mountedData.erase(dataIterator);
        return true;
    }

    std::string realPath;
    std::string sourceBase = this->GetSourceBaseDirectory();

    // Check whether the given archive path is in the list of allowed full paths.
    auto iterator =
        std::find(this->allowedMountPaths.begin(), this->allowedMountPaths.end(), archive);

    // Special case: if the game is fused and the archive is the source's
    // base directory, unmount it even though it's outside of the save dir.

    if (iterator != this->allowedMountPaths.end())
        realPath = *iterator;
    else if (this->IsFused() && sourceBase.compare(archive) == 0)
        realPath = sourceBase;
    else
    {
        // Don't allow these, not safe
        if (strlen(archive) == 0 || strstr(archive, "..") || strcmp(archive, "/") == 0)
            return false;

        const char* realDirectory = PHYSFS_getRealDir(archive);

        if (!realDirectory)
            return false;

        realPath = realDirectory;

        if (realPath.find(this->gameSource) == 0)
            return false;

        realPath += LOVE_PATH_SEPARATOR;
        realPath += archive;
    }

    const char* mountPoint = PHYSFS_getMountPoint(realPath.c_str());

    if (!mountPoint)
        return false;

    return PHYSFS_unmount(realPath.c_str()) != 0;
}

bool Filesystem::UnMount(Data* data)
{
    for (const auto& dataPair : this->mountedData)
    {
        if (dataPair.second.Get() == data)
        {
            std::string archive = dataPair.first;
            return this->UnMount(archive.c_str());
        }
    }

    return false;
}

std::vector<std::string>& Filesystem::GetRequirePath()
{
    return this->requirePath;
}

std::string Filesystem::GetUserDirectory()
{
    static std::string userDirectory = normalize(PHYSFS_getUserDir());

    return userDirectory;
}

bool Filesystem::SetIdentity(const char* name, bool appendToPath)
{
    if (!PHYSFS_isInit())
        return false;

    std::string oldSavePath = this->fullSavePath;

    // Save directory
    this->identity = name;

    // /{game}
    this->relativeSavePath =
        std::string(LOVE_APPDATA_PREFIX LOVE_APPDATA_FOLDER LOVE_PATH_SEPARATOR) + this->identity;

    // sdmc:/switch/LovePotion
    this->fullSavePath = this->GetAppDataDirectory() + LOVE_PATH_SEPARATOR;

    if (this->fused) // sdmc:/switch/{game}/save
        this->fullSavePath += "save";
    else // sdmc:/switch/LovePotion/save/{game}
        this->fullSavePath += std::string("save" LOVE_PATH_SEPARATOR) + this->relativeSavePath;

    this->fullSavePath = normalize(this->fullSavePath);

    if (!oldSavePath.empty())
        PHYSFS_unmount(oldSavePath.c_str());

    PHYSFS_mount(this->fullSavePath.c_str(), nullptr, appendToPath);

    PHYSFS_setWriteDir(nullptr);

    return true;
}

std::string Filesystem::GetAppDataDirectory()
{
    return this->GetUserDirectory();
}

void Filesystem::Write(const char* filename, const void* data, int64_t size)
{
    File file(filename);

    file.Open(File::MODE_WRITE);
    if (!file.Write(data, size))
        throw love::Exception("Data could not be written.");
}

void Filesystem::AllowMountingForPath(const std::string& path)
{
    if (std::find(this->allowedMountPaths.begin(), this->allowedMountPaths.end(), path) ==
        this->allowedMountPaths.end())
        this->allowedMountPaths.push_back(path);
}

bool Filesystem::GetConstant(const char* in, FileType& out)
{
    return fileTypes.Find(in, out);
}

bool Filesystem::GetConstant(FileType in, const char*& out)
{
    return fileTypes.Find(in, out);
}

std::vector<const char*> Filesystem::GetConstants(FileType)
{
    return fileTypes.GetNames();
}

// clang-format off
constexpr StringMap<Filesystem::FileType, Filesystem::FILETYPE_MAX_ENUM>::Entry fileTypeEntries[] =
{
    { "file",      Filesystem::FileType::FILETYPE_FILE      },
    { "directory", Filesystem::FileType::FILETYPE_DIRECTORY },
    { "symlink",   Filesystem::FileType::FILETYPE_SYMLINK   },
    { "other",     Filesystem::FileType::FILETYPE_OTHER     },
};

constinit const StringMap<Filesystem::FileType, Filesystem::FILETYPE_MAX_ENUM> Filesystem::fileTypes(fileTypeEntries);
// clang-format on
