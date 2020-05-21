#include "common/runtime.h"
#include "modules/filesystem/filesystem.h"

#include "common/assets.h"
#include <sys/stat.h>

#include <physfs.h>

using namespace love;

love::Type Filesystem::type("Filesystem", &Module::type);

Filesystem::Filesystem() : identity("SuperGame"),
                           fused(false)
{}

void Filesystem::Init(const char * arg0)
{
    if (!PHYSFS_init(arg0))
        throw love::Exception("Failed to initialize filesystem: %s", PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode()));
}

Filesystem::~Filesystem()
{
    if (PHYSFS_isInit())
        PHYSFS_deinit();
}

void Filesystem::Append(const char * filename, const void * data, int64_t size)
{
    File file(filename);
    file.Open(File::MODE_APPEND);

    if (!file.Write(data, size))
        throw love::Exception("Data could not be written.");
}

void Filesystem::CreateDirectory(const char * name)
{
    std::filesystem::create_directory(this->GetSaveDirectory() + name);
}

void Filesystem::GetDirectoryItems(const char * directory, std::vector<std::string> & items)
{
    if (!PHYSFS_isInit())
        return;

    char ** results = PHYSFS_enumerateFiles(directory);

    if (results == nullptr)
        return;

    for (char ** item = results; *item != 0; item++)
        items.push_back(*item);

    PHYSFS_freeList(results);
}

bool Filesystem::SetupWriteDirectory()
{
    if (this->identity.empty() || this->fullSavePath.empty() || this->relativeSavePath.empty())
        return false;

    if (!std::filesystem::create_directory(this->fullSavePath))
        return false;

    return true;
}

const char * Filesystem::GetIdentity()
{
    return this->identity.c_str();
}

bool Filesystem::GetInfo(const char * filename, Filesystem::Info & info) const
{
    struct stat fileInfo;

    if (stat(filename, &fileInfo) != 0)
        return false;

    auto statType = fileInfo.st_mode;

    if (S_ISREG(statType))
        info.type = FILETYPE_FILE;
    else if (S_ISDIR(statType))
        info.type = FILETYPE_DIRECTORY;
    else if (S_ISLNK(statType))
        info.type = FILETYPE_SYMLINK;
    else
        info.type = FILETYPE_OTHER;

    info.modtime = std::min<int64_t>(fileInfo.st_mtime, MAX_STAMP);
    info.size = std::min<int64_t>(fileInfo.st_size, MAX_STAMP);

    return true;
}

std::string Filesystem::GetSaveDirectory()
{
    return this->fullSavePath;
}

File * Filesystem::NewFile(const char * filename)
{
    return new File(filename);
}

FileData * Filesystem::NewFileData(const void * data, size_t size, const char * filename)
{
    FileData * fileData = new FileData(size, filename);
    memcpy(fileData->GetData(), data, size);

    return fileData;
}

FileData * Filesystem::Read(const char * filename, int64_t size)
{
    File file(filename);

    file.Open(File::MODE_READ);

    return file.Read(size);
}

bool Filesystem::Remove(const char * filename)
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
    // This is likely to be wrong: FIX

    char buffer[LOVE_MAX_PATH];

    return getcwd(buffer, LOVE_MAX_PATH);
}

void Filesystem::SetFused(bool fused)
{
    if (this->fusedSet)
        return;

    this->fused = fused;
    this->fusedSet = true;
}

const char * Filesystem::GetWorkingDirectory()
{
    if (this->cwd.empty())
    {
        char * cwd_char = new char[LOVE_MAX_PATH];

        if (getcwd(cwd_char, LOVE_MAX_PATH))
            this->cwd = cwd_char;

        delete[] cwd_char;
    }

    return this->cwd.c_str();
}

bool Filesystem::IsFused() const
{
    if (!this->fusedSet)
        return false;

    return this->fused;
}

const char * Filesystem::GetSource() const
{
    return this->gameSource.c_str();
}

bool Filesystem::SetSource(const char * source)
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

std::string Filesystem::getSourceBaseDirectory() const
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

bool Filesystem::Mount(const char * archive, const char * mountpoint, bool appendToPath = false)
{
    if (!PHYSFS_isInit() || !archive)
        return false;

    std::string realPath;
    std::string sourceBase = this->GetSourceBaseDirectory();

    auto iterator = std::find(this->allowedMountPaths.begin(), this->allowedMountPaths.end(), archive);

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
        if (strlen(archive) == 0 || strstr(archive, "..") || strncmp(acrhive, "/", 1) == 0)
            return false;

        const char * realDirectory = PHYSFS_getRealDir(archive);

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

bool Filesystem::SetIdentity(const char * name, bool appendToPath)
{
    if (!PHYSFS_isInit())
        return false;

    std::string oldSavePath = this->fullSavePath;

    this->identity = name;

    this->relativeSavePath = std::string(LOVE_APPDATA_PREFIX LOVE_APPDATA_FOLDER LOVE_PATH_SEPARATOR) + this->identity;

    this->fullSavePath = this->GetAppDataDirectory() + LOVE_PATH_SEPARATOR;

    if (this->fused)
        this->fullSavePath += std::string(LOVE_APPDATA_PREFIX) + this->identity;
    else
        this->fullSavePath += this->relativeSavePath;

    // this->searchPaths.push_back(this->fullSavePath);

    if (!oldSavePath.empty())
        PHYSFS_unmount(oldSavePath.c_str());

    PHYSFS_mount(this->fullSavePath.c_str(), nullptr, appendToPath);

    PHYSFS_setWriteDir(nullptr);

    return true;
}

std::string Filesystem::GetAppDataDirectory()
{
    if (this->appdata.empty())
        this->appdata = getcwd(NULL, LOVE_MAX_PATH);

    return this->appdata;
}

void Filesystem::Write(const char * filename, const void * data, int64_t size)
{
    File file(filename);

    file.Open(File::MODE_WRITE);
    if (!file.Write(data, size))
        throw love::Exception("Data could not be written.");
}

void Filesystem::AllowMountingForPath(const std::string & path)
{
    if (std::find(this->allowedMountPaths.begin(), this->allowedMountPaths.end(), path) == this->allowedMountPaths.end())
        this->allowedMountPaths.push_back(path);
}

bool Filesystem::GetConstant(const char * in, FileType & out)
{
    return fileTypes.Find(in, out);
}

bool Filesystem::GetConstant(FileType in, const char *& out)
{
    return fileTypes.Find(in, out);
}

std::vector<std::string> Filesystem::getConstants(FileType)
{
    return fileTypes.GetNames();
}

StringMap<Filesystem::FileType, Filesystem::FILETYPE_MAX_ENUM>::Entry Filesystem::fileTypeEntries[] =
{
    { "file",      FILETYPE_FILE      },
    { "directory", FILETYPE_DIRECTORY },
    { "symlink",   FILETYPE_SYMLINK   },
    { "other",     FILETYPE_OTHER     },
};

StringMap<Filesystem::FileType, Filesystem::FILETYPE_MAX_ENUM> Filesystem::fileTypes(Filesystem::fileTypeEntries, sizeof(Filesystem::fileTypeEntries));
