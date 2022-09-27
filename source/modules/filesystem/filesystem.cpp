#include <common/console.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <algorithm>
#include <filesystem>

#include <modules/filesystem/filesystem.hpp>

using namespace love;

FileData* Filesystem::NewFileData(const void* data, size_t size, const char* filename) const
{
    auto* fileData = new FileData(size, filename);
    std::copy_n((const uint8_t*)data, size, (uint8_t*)fileData->GetData());

    return fileData;
}

bool Filesystem::GetRealPathType(const std::string& path, FileType& type) const
{
    if (!std::filesystem::exists(path))
        return false;

    if (std::filesystem::is_regular_file(path))
        type = FILETYPE_FILE;
    else if (std::filesystem::is_directory(path))
        type = FILETYPE_DIRECTORY;
    else if (std::filesystem::is_symlink(path))
        type = FILETYPE_SYMLINK;
    else
        type = FILETYPE_OTHER;

    return true;
}

bool Filesystem::IsRealDirectory(const std::string& path) const
{
    auto type = FileType::FILETYPE_MAX_ENUM;

    if (!this->GetRealPathType(path, type))
        return false;

    return (type == FileType::FILETYPE_DIRECTORY);
}

bool Filesystem::CreateRealDirectory(const std::string& path)
{
    auto type = FileType::FILETYPE_MAX_ENUM;

    if (this->GetRealPathType(path, type))
        return type == FileType::FILETYPE_DIRECTORY;

    return std::filesystem::create_directories(path);
}

std::string Filesystem::GetExecutablePath() const
{
    return this->executablePath;
}

// clang-format off
constexpr BidirectionalMap fileTypes = {
    "file",      Filesystem::FileType::FILETYPE_FILE,
    "directory", Filesystem::FileType::FILETYPE_DIRECTORY,
    "symlink",   Filesystem::FileType::FILETYPE_SYMLINK,
    "other",     Filesystem::FileType::FILETYPE_OTHER
};
// clang-format on

bool Filesystem::GetConstant(const char* in, FileType& out)
{
    return fileTypes.Find(in, out);
}

bool Filesystem::GetConstant(FileType in, const char*& out)
{
    return fileTypes.ReverseFind(in, out);
}

SmallTrivialVector<const char*, 4> Filesystem::GetConstants(FileType)
{
    return fileTypes.GetNames();
}

// clang-format off
constexpr BidirectionalMap commonPaths = {
    "appsavedir",    Filesystem::APP_SAVEDIR,
    "appdocuments",  Filesystem::APP_DOCUMENTS,
    "userhome",      Filesystem::USER_HOME,
    "userdocuments", Filesystem::USER_DOCUMENTS
};
// clang-format on

bool Filesystem::GetConstant(const char* in, CommonPath& out)
{
    return commonPaths.Find(in, out);
}

bool Filesystem::GetConstant(CommonPath in, const char*& out)
{
    return commonPaths.ReverseFind(in, out);
}

SmallTrivialVector<const char*, 4> Filesystem::GetConstants(CommonPath)
{
    return commonPaths.GetNames();
}

// clang-format off
constexpr BidirectionalMap mountPermissions = {
    "read",      Filesystem::MOUNT_READ,
    "readwrite", Filesystem::MOUNT_READWRITE
};
// clang-format on

bool Filesystem::GetConstant(const char* in, MountPermissions& out)
{
    return mountPermissions.Find(in, out);
}

bool Filesystem::GetConstant(MountPermissions in, const char*& out)
{
    return mountPermissions.ReverseFind(in, out);
}

SmallTrivialVector<const char*, 2> Filesystem::GetConstants(MountPermissions)
{
    return mountPermissions.GetNames();
}
