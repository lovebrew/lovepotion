#include "modules/filesystem/Filesystem.tcc"

namespace love
{
    bool FilesystemBase::isRealDirectory(const std::string& path) const
    {
        FileType type = FILETYPE_MAX_ENUM;
        if (!this->getRealPathType(path, type))
            return false;

        return type == FILETYPE_DIRECTORY;
    }

    bool FilesystemBase::getRealPathType(const std::string& path, FileType& type) const
    {
        struct stat buffer;
        if (stat(path.c_str(), &buffer) != 0)
            return false;

        if (S_ISREG(buffer.st_mode))
            type = FILETYPE_FILE;
        else if (S_ISDIR(buffer.st_mode))
            type = FILETYPE_DIRECTORY;
        else if (S_ISLNK(buffer.st_mode))
            type = FILETYPE_SYMLINK;
        else
            type = FILETYPE_OTHER;

        return true;
    }

    static bool getContainingDirectory(const std::string& path, std::string& newPath)
    {
        size_t index = path.find_last_of("/\\");

        if (index == std::string::npos)
            return false;

        newPath = path.substr(0, index);
        return newPath.find_first_of("/\\") != std::string::npos;
    }

    static bool createDirectoryRaw(const std::string& path)
    {
        return mkdir(path.c_str(), S_IRWXU) == 0;
    }

    bool FilesystemBase::createRealDirectory(const std::string& path)
    {
        FileType type = FILETYPE_MAX_ENUM;
        if (this->getRealPathType(path, type))
            return type == FILETYPE_DIRECTORY;

        std::vector<std::string> createPaths = { path };

        while (true)
        {
            std::string subPath {};
            if (!getContainingDirectory(createPaths[0], subPath))
                break;

            if (this->isRealDirectory(subPath))
                break;

            createPaths.insert(createPaths.begin(), subPath);
        }

        for (const std::string& createPath : createPaths)
        {
            if (!createDirectoryRaw(createPath))
                return false;
        }

        return true;
    }

    std::string FilesystemBase::canonicalizeRealPath(const std::string& path) const
    {
        try
        {
            return std::filesystem::weakly_canonical(path).string();
        }
        catch (std::exception&)
        {
            return path;
        }
    }

    FileData* FilesystemBase::newFileData(const void* data, size_t size, const std::string& filename) const
    {
        FileData* fileData = new FileData(size, filename);
        std::memcpy(fileData->getData(), data, size);

        return fileData;
    }
} // namespace love
