#include <common/console.hpp>
#include <modules/filesystem/physfs/filesystem.hpp>

#include <physfs.h>

#if defined(__3DS__)
    #include <3ds.h>
#elif defined(__SWITCH__)
    #include <switch.h>
#elif defined(__WIIU__)
    #include <coreinit/dynload.h>
#endif

#include <unistd.h>

#include <algorithm>
#include <filesystem>

#define APPDATA_FOLDER ""
#define APPDATA_PREFIX ""
#define PATH_SEPARATOR "/"

#define normalize(x) std::string(std::filesystem::path((x)).lexically_normal())
#define parentize(x) std::string(std::filesystem::path((x)).parent_path())

using namespace love::physfs;

#include <utilities/log/logfile.hpp>

static std::string getApplicationPath(std::string origin)
{
#if defined(__EMULATION__)
    if (love::Console::Is(love::Console::CTR))
        return "sdmc:/lovepotion.3dsx";
    else if (love::Console::Is(love::Console::HAC))
        return "sdmc:/lovepotion.nro";
    else
        return "fs:/vol/external01/wiiu/apps/lovepotion/lovepotion.wuhb";
#endif

    if (!love::Console::Is(love::Console::CAFE))
        return origin;

#if defined(__WIIU__)
    OSDynLoad_Module module;
    const auto success = 0;
    const auto name    = "RL_GetPathOfRunningExecutable";

    if (OSDynLoad_Acquire("homebrew_rpx_loader", &module) == OS_DYNLOAD_OK)
    {
        char path[256];

        bool (*RL_GetPathOfRunningExecutable)(char*, uint32_t);
        auto** ptrFunction = reinterpret_cast<void**>(&RL_GetPathOfRunningExecutable);

        // clang-format off
        if (OSDynLoad_FindExport(module, OS_DYNLOAD_EXPORT_FUNC, name, ptrFunction) == OS_DYNLOAD_OK)
        {
            if (RL_GetPathOfRunningExecutable(path, sizeof(path)) == success)
                return path;
        }
        // clang-format on
    }
    return std::string {};
#endif
}

Filesystem::Filesystem() :
    appendIdentityToPath(false),
    fused(false),
    fusedSet(false),
    fullPaths(),
    commonPathMountInfo(),
    saveDirectoryNeedsMounting(false),
    appCommonPaths({ CommonPath::APP_SAVEDIR, CommonPath::APP_DOCUMENTS })
{
    this->requirePath = { "?.lua", "?/init.lua" };
}

Filesystem::~Filesystem()
{
    if (PHYSFS_isInit())
        PHYSFS_deinit();
}

/*
** get the last physfs error as a string
** can be cleared by passing true to args
*/
const char* Filesystem::GetLastError(bool clear)
{
    if (clear)
    {
        PHYSFS_getLastErrorCode();
        return nullptr;
    }

    return PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode());
}

bool Filesystem::SetupWriteDirectory()
{
    if (!PHYSFS_isInit())
        return false;

    if (!this->saveDirectoryNeedsMounting)
        return true;

    if (this->saveIdentity.empty())
        return false;

    bool create = true;

    auto savePath    = CommonPath::APP_SAVEDIR;
    auto permissions = MountPermissions::MOUNT_READWRITE;

    if (!this->MountCommonPathInternal(savePath, nullptr, permissions, this->appendIdentityToPath,
                                       create))
    {
        return false;
    }

    this->saveDirectoryNeedsMounting = false;

    return true;
}

bool Filesystem::IsAppCommonPath(CommonPath path)
{
    switch (path)
    {
        case CommonPath::APP_SAVEDIR:
        case CommonPath::APP_DOCUMENTS:
            return true;
        default:
            return false;
    }
}

void Filesystem::Init(const char* arg0)
{
    this->executablePath = getApplicationPath(arg0);

    /* should only happen on Wii U */
    if (this->executablePath.empty())
        throw love::Exception("Failed to get executable path.");

    if (!PHYSFS_init(this->executablePath.c_str()))
        throw love::Exception("Failed to initialize filesystem: %s", Filesystem::GetLastError());

    PHYSFS_setWriteDir(nullptr);

    this->SetSymlinksEnabled(false);
}

void Filesystem::SetFused(bool fused)
{
    if (this->fusedSet)
        return;

    this->fused    = fused;
    this->fusedSet = true;
}

bool Filesystem::IsFused() const
{
    if (!this->fusedSet)
        return false;

    return this->fused;
}

bool Filesystem::SetIdentity(const char* identity, bool appendToPath)
{
    if (!PHYSFS_isInit())
        return false;

    if (identity == nullptr || strlen(identity) == 0)
        return false;

    for (auto path : this->appCommonPaths)
    {
        if (!this->commonPathMountInfo[path].mounted)
            continue;

        std::string fullPath = this->GetFullCommonPath(path);

        if (!fullPath.empty() && !PHYSFS_unmount(fullPath.c_str()))
            return false;
    }

    std::array<bool, CommonPath::PATH_MAX_ENUM> oldMountedCommonPaths = { false };

    /* prevent save paths from accumulating on new identity sets */
    for (auto path : this->appCommonPaths)
    {
        oldMountedCommonPaths[path] = this->commonPathMountInfo[path].mounted;

        if (this->commonPathMountInfo[path].mounted)
            this->UnMount(path);
    }

    /* repopulated on Filesystem::GetFullCommonPath */
    for (auto path : this->appCommonPaths)
        this->fullPaths[path].clear();

    this->saveIdentity         = std::string(identity);
    this->appendIdentityToPath = appendToPath;

    /* mount the save directory */
    auto savePath   = CommonPath::APP_SAVEDIR;
    auto permission = MountPermissions::MOUNT_READWRITE;

    if (!this->MountCommonPathInternal(savePath, nullptr, permission, appendToPath, false))
        this->saveDirectoryNeedsMounting = true;

    /* mount the remaining common paths */
    for (auto path : this->appCommonPaths)
    {
        if (oldMountedCommonPaths[path] && path != CommonPath::APP_SAVEDIR)
        {
            auto info = this->commonPathMountInfo[path];
            this->MountCommonPathInternal(path, info.mountPoint.c_str(), info.permissions,
                                          appendToPath, true);
        }
    }

    return true;
}

std::string Filesystem::GetIdentity() const
{
    return this->saveIdentity;
}

bool Filesystem::SetSource(const char* source)
{
    if (!PHYSFS_isInit())
        return false;

    if (!this->gameSource.empty())
        return false;

    std::string search = source;
    if (!PHYSFS_mount(search.c_str(), nullptr, 1))
        return false;

    this->gameSource = search;

    return true;
}

std::string Filesystem::GetSource() const
{
    return this->gameSource;
}

std::string Filesystem::GetFullCommonPath(CommonPath path)
{
    if (!this->fullPaths[path].empty())
        return this->fullPaths[path];

    if (this->IsAppCommonPath(path))
    {
        if (this->saveIdentity.empty())
            return this->fullPaths[path];

        std::string root {};

        switch (path)
        {
            case CommonPath::APP_SAVEDIR:
            {
                root = this->GetFullCommonPath(CommonPath::USER_APPDATA);
                break;
            }
            case CommonPath::APP_DOCUMENTS:
                root = this->GetFullCommonPath(CommonPath::USER_DOCUMENTS);
                break;
            default:
                break;
        }

        if (root.empty())
            return this->fullPaths[path];

        std::string suffix {};

        if (this->IsFused())
            suffix = std::string(PATH_SEPARATOR) + this->saveIdentity;
        else
            suffix = std::string(PATH_SEPARATOR APPDATA_FOLDER PATH_SEPARATOR) + this->saveIdentity;

        this->fullPaths[path] = normalize(root + suffix);

        return this->fullPaths[path];
    }

    switch (path)
    {
        case CommonPath::APP_SAVEDIR:
        case CommonPath::APP_DOCUMENTS:
            break;
        case CommonPath::USER_HOME:
        {
            fullPaths[path] = normalize(PHYSFS_getUserDir());
            break;
        }
        case CommonPath::USER_APPDATA:
        {
            if (Console::Is(Console::CAFE))
                this->fullPaths[path] = normalize(parentize(this->executablePath)) + "/save/";
            else
                this->fullPaths[path] = normalize(std::string(PHYSFS_getUserDir()) + "/save/");

            break;
        }
        case CommonPath::USER_DOCUMENTS:
        {
            this->fullPaths[path] = normalize(PHYSFS_getUserDir());
            break;
        }
        case CommonPath::PATH_MAX_ENUM:
        default:
            break;
    }

    return this->fullPaths[path];
}

bool Filesystem::MountCommonPathInternal(CommonPath path, const char* mountPoint,
                                         MountPermissions permissions, bool appendToPath,
                                         bool createDirectory)
{
    std::string fullPath = this->GetFullCommonPath(path);

    if (fullPath.empty())
        return false;

    if (createDirectory && this->IsAppCommonPath(path) && !this->IsRealDirectory(fullPath))
    {
        if (!this->CreateRealDirectory(fullPath))
            return false;
    }

    if (this->MountFullPath(fullPath.c_str(), mountPoint, permissions, appendToPath))
    {
        std::string mount               = (mountPoint != nullptr) ? mountPoint : "/";
        this->commonPathMountInfo[path] = { true, mount, permissions };

        return true;
    }

    return false;
}

bool Filesystem::MountCommonPath(CommonPath path, const char* mountPoint,
                                 MountPermissions permissions, bool appendToPath)
{
    return this->MountCommonPathInternal(path, mountPoint, permissions, appendToPath, true);
}

bool Filesystem::MountFullPath(const char* archive, const char* mountPoint,
                               MountPermissions permissions, bool appendToPath)
{
    if (!PHYSFS_isInit() || !archive)
        return false;

    /* temp hack */
    if (permissions == MountPermissions::MOUNT_READWRITE)
    {
        if (!PHYSFS_setWriteDir(archive))
            return false;
    }

    // if (permissions == MountPermissions::READWRITE)
    //     return PHYSFS_mountRW(archive, mountPoint, appendToPath) != 0;
    return PHYSFS_mount(archive, mountPoint, appendToPath) != 0;
}

bool Filesystem::Mount(const char* archive, const char* mountPoint, bool appendToPath)
{
    if (!PHYSFS_isInit() || !archive)
        return false;

    std::string realPath {};
    std::string sourceBase = this->GetSourceBaseDirectory();

    auto allowedIterator =
        std::find(this->allowedMountPaths.begin(), this->allowedMountPaths.end(), archive);

    if (allowedIterator != allowedMountPaths.end())
        realPath = *allowedIterator;
    else if (this->IsFused() && sourceBase.compare(archive) == 0)
        realPath = sourceBase;
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

        realPath += std::string(PATH_SEPARATOR) + archive;
    }

    const auto permissions = MountPermissions::MOUNT_READ;
    return this->MountFullPath(realPath.c_str(), mountPoint, permissions, appendToPath);
}

bool Filesystem::Mount(Data* data, const char* archiveName, const char* mountPoint,
                       bool appendToPath)
{
    if (!PHYSFS_isInit())
        return false;

    if (PHYSFS_mountMemory(data->GetData(), data->GetSize(), nullptr, archiveName, mountPoint,
                           appendToPath))
    {
        this->mountedData[archiveName] = data;

        return true;
    }

    return false;
}

bool Filesystem::UnMount(const char* archive)
{
    if (!PHYSFS_isInit() || !archive)
        return false;

    auto mountedIterator = this->mountedData.find(archive);

    if (mountedIterator != mountedData.end() && PHYSFS_unmount(archive) != 0)
    {
        this->mountedData.erase(mountedIterator);

        return true;
    }

    auto allowedIterator = std::find(allowedMountPaths.begin(), allowedMountPaths.end(), archive);

    if (allowedIterator != allowedMountPaths.end())
        return this->UnMountFullPath(archive);

    std::string sourceBase = this->GetSourceBaseDirectory();

    if (this->IsFused() && sourceBase.compare(archive) == 0)
        return this->UnMountFullPath(archive);

    if (strlen(archive) == 0 || strstr(archive, "..") || strcmp(archive, "/") == 0)
        return false;

    const char* realDirectory = PHYSFS_getRealDir(archive);

    if (!realDirectory)
        return false;

    std::string realPath = realDirectory;
    realPath += std::string(PATH_SEPARATOR) + archive;

    if (PHYSFS_getMountPoint(realPath.c_str()) == nullptr)
        return false;

    return (PHYSFS_unmount(realPath.c_str()) != 0);
}

bool Filesystem::UnMountFullPath(const char* fullPath)
{
    if (!PHYSFS_isInit() || !fullPath)
        return false;

    return (PHYSFS_unmount(fullPath) != 0);
}

bool Filesystem::UnMount(CommonPath path)
{
    std::string fullPath = this->GetFullCommonPath(path);

    if (!fullPath.empty() && this->UnMountFullPath(fullPath.c_str()))
    {
        this->commonPathMountInfo[path].mounted = false;

        return true;
    }

    return false;
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

love::File* Filesystem::OpenFile(const char* filename, File::Mode mode) const
{
    return new File(filename, mode);
}

std::string Filesystem::GetWorkingDirectory()
{
    if (this->currentDirectory.empty())
    {
        char* cwdInfo = new char[PATH_MAX];

        if (getcwd(cwdInfo, PATH_MAX))
            this->currentDirectory = cwdInfo;

        delete[] cwdInfo;

        if (Console::Is(Console::CAFE))
            this->currentDirectory = parentize(getApplicationPath(""));
    }

    return this->currentDirectory;
}

std::string Filesystem::GetUserDirectory()
{
    return this->GetFullCommonPath(CommonPath::USER_HOME);
}

std::string Filesystem::GetAppdataDirectory()
{
    return this->GetFullCommonPath(CommonPath::USER_APPDATA);
}

std::string Filesystem::GetSaveDirectory()
{
    return this->GetFullCommonPath(CommonPath::APP_SAVEDIR);
}

std::string Filesystem::GetSourceBaseDirectory() const
{
    const auto asFilesystemPath = std::filesystem::path(this->gameSource);

    if (!asFilesystemPath.has_parent_path())
        return std::string {};

    const auto newPath = asFilesystemPath.parent_path();

    if (!newPath.has_parent_path())
        return std::string {};

    return newPath.parent_path();
}

std::string Filesystem::GetRealDirectory(const char* filename) const
{
    if (!PHYSFS_isInit())
        throw love::Exception("PhysFS is not initialized.");

    const char* directory = PHYSFS_getRealDir(filename);

    if (directory == nullptr)
        throw love::Exception("File does not exist on disk.");

    return std::string(directory);
}

bool Filesystem::Exists(const char* filepath) const
{
    if (!PHYSFS_isInit())
        return false;

    return PHYSFS_exists(filepath) != 0;
}

bool Filesystem::GetInfo(const char* filepath, Info& info) const
{
    if (!PHYSFS_isInit())
        return false;

    PHYSFS_Stat stat {};

    if (!PHYSFS_stat(filepath, &stat))
        return false;

    info.size     = stat.filesize;
    info.modtime  = stat.modtime;
    info.readOnly = stat.readonly;

    if (stat.filetype == PHYSFS_FILETYPE_REGULAR)
        info.type = FileType::FILETYPE_FILE;
    else if (stat.filetype == PHYSFS_FILETYPE_DIRECTORY)
        info.type = FileType::FILETYPE_DIRECTORY;
    else if (stat.filetype == PHYSFS_FILETYPE_SYMLINK)
        info.type = FileType::FILETYPE_SYMLINK;
    else
        info.type = FileType::FILETYPE_OTHER;

    return true;
}

bool Filesystem::CreateDirectory(const char* directory)
{
    if (!PHYSFS_isInit())
        return false;

    if (!this->SetupWriteDirectory())
        return false;

    if (!PHYSFS_mkdir(directory))
        return false;

    return true;
}

bool Filesystem::Remove(const char* file)
{
    if (!PHYSFS_isInit())
        return false;

    if (!this->SetupWriteDirectory())
        return false;

    if (!PHYSFS_delete(file))
        return false;

    return true;
}

love::FileData* Filesystem::Read(const char* filename, int64_t size) const
{
    File file(filename, File::MODE_READ);

    return file.Read(size);
}

love::FileData* Filesystem::Read(const char* filename) const
{
    File file(filename, File::MODE_READ);

    return file.Read();
}

void Filesystem::Write(const char* filename, const void* data, int64_t size) const
{
    File file(filename, File::MODE_WRITE);

    if (!file.Write(data, size))
        throw love::Exception("Data could not be written.");
}

void Filesystem::Append(const char* filename, const void* data, int64_t size) const
{
    File file(filename, File::MODE_APPEND);

    if (!file.Write(data, size))
        throw love::Exception("Data could not be written.");
}

bool Filesystem::GetDirectoryItems(const char* directory, std::vector<std::string>& items)
{
    if (!PHYSFS_isInit())
        return false;

    char** files = PHYSFS_enumerateFiles(directory);

    if (files == nullptr)
        return false;

    for (auto file = files; *file != nullptr; file++)
        items.push_back(*file);

    PHYSFS_freeList(files);

    return true;
}

void Filesystem::SetSymlinksEnabled(bool enable)
{
    if (!PHYSFS_isInit())
        return;

    PHYSFS_permitSymbolicLinks(enable ? 1 : 0);
}

bool Filesystem::AreSymlinksEnabled() const
{
    if (!PHYSFS_isInit())
        return false;

    return (PHYSFS_symbolicLinksPermitted() != 0);
}

std::vector<std::string>& Filesystem::GetRequirePath()
{
    return this->requirePath;
}

void Filesystem::AllowMountingForPath(const std::string& path)
{
    auto iterator = std::find(allowedMountPaths.begin(), allowedMountPaths.end(), path);

    if (iterator == allowedMountPaths.end())
        allowedMountPaths.push_back(path);
}
