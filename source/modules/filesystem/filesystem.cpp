#include "common/runtime.h"
#include "modules/filesystem/filesystem.h"

#include "common/assets.h"
#include <sys/stat.h>

using namespace love;

love::Type Filesystem::type("Filesystem", &Module::type);

Filesystem::Filesystem() : identity("SuperGame")
{}

void Filesystem::Append(const char * filename, const void * data, int64_t size)
{
    File file(filename);
    file.Open(File::MODE_APPEND);

    if (!file.Write(data, size))
        throw love::Exception("Data could not be written.");
}

void Filesystem::CreateDirectory(const char * name)
{
    std::filesystem::create_directory(name);
}

void Filesystem::GetDirectoryItems(const char * directory, std::vector<std::string> & items)
{
    auto iterator = std::filesystem::directory_iterator(directory);

    for (auto & entry : iterator)
        items.emplace_back(entry.path().filename());
}

std::string Filesystem::GetIdentity()
{
    return this->identity;
}

bool Filesystem::GetInfo(const char * filename, Filesystem::Info & info) const
{
    struct stat fileInfo;

    if (stat(filename, &fileInfo) != 0)
        return false;

    auto statType = fileInfo.st_mode;

    if (S_ISREG(statType))
        info.type = FileType::FILE;
    else if (S_ISDIR(statType))
        info.type = FileType::DIRECTORY;
    else if (S_ISLNK(statType))
        info.type = FileType::SYMLINK;
    else
        info.type = FileType::OTHER;

    info.modtime = std::min<int64_t>(fileInfo.st_mtime, MAX_STAMP);
    info.size = std::min<int64_t>(fileInfo.st_size, MAX_STAMP);

    return true;
}

std::string Filesystem::GetSaveDirectory()
{
    return Assets::GetWritePath();
}

File * Filesystem::NewFile(const char * filename)
{
    return new File(filename);
}

FileData * Filesystem::NewFileData(const void * data, size_t size, const char * filename)
{
    FileData * fileData = new FileData(size, std::string(filename));
    memcpy(fileData->GetData(), data, size);

    return fileData;
}

FileData * Filesystem::Read(const char * filename, int64_t size)
{
    File file(this->Redirect(filename));

    file.Open(File::MODE_READ);

    return file.Read(size);
}

bool Filesystem::Remove(const char * filename)
{
    return std::filesystem::remove(this->GetSaveDirectory() + filename);
}

void Filesystem::SetIdentity(const char * name)
{
    this->identity = name;
}

void Filesystem::Write(const char * filename, const void * data, int64_t size)
{
    File file(this->GetSaveDirectory() + filename);

    file.Open(File::MODE_WRITE);

    if (!file.Write(data, size))
        throw love::Exception("Data could not be written.");
}

/*
** Check save directory first
** If the file doesn't exist, return source directory
*/
std::string Filesystem::Redirect(const char * path)
{
    if (std::filesystem::exists(GetSaveDirectory() + std::string(path)))
        return GetSaveDirectory() + std::string(path);
    else
        return path;
}
