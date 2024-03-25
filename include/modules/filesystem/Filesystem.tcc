#pragma once

#include "common/Module.hpp"
#include "common/int.hpp"

#include "modules/filesystem/FileData.hpp"

#include "utility/map.hpp"

#include <filesystem>
#include <string>
#include <vector>

#include <sys/stat.h>

#include <cstring>

namespace love
{
    template<class T>
    class FilesystemBase : public Module
    {
      private:
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

      public:
        enum FileType
        {
            FILETYPE_FILE,
            FILETYPE_DIRECTORY,
            FILETYPE_SYMLINK,
            FILETYPE_OTHER,
            FILETYPE_MAX_ENUM
        };

        enum CommonPath
        {
            COMMONPATH_APP_SAVEDIR,
            COMMONPATH_APP_DOCUMENTS,
            COMMONPATH_USER_HOME,
            COMMONPATH_USER_APPDATA,
            COMMONPATH_USER_DESKTOP,
            COMMONPATH_USER_DOCUMENTS,
            COMMONPATH_MAX_ENUM
        };

        enum MountPermissions
        {
            MOUNT_PERMISSIONS_READ,
            MOUNT_PERMISSIONS_READWRITE,
            MOUNT_PERMISSIONS_MAX_ENUM
        };

        enum LoadMode
        {
            LOADMODE_BINARY,
            LOADMODE_TEXT,
            LOADMODE_ANY,
            LOADMODE_MAX_ENUM
        };

        struct Info
        {
            int64_t size;
            int64_t modtime;
            FileType type;
            bool readonly;
        };

        FilesystemBase(const char* name) : Module(M_FILESYSTEM, name)
        {}

        virtual ~FilesystemBase()
        {}

        bool isRealDirectory(const std::string& path) const
        {
            FileType type = FILETYPE_MAX_ENUM;
            if (!this->getRealPathType(path, type))
                return false;

            return type == FILETYPE_DIRECTORY;
        }

        bool getRealPathType(const std::string& path, FileType& type) const
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

        bool createRealDirectory(const std::string& path)
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

        std::string getExecutablePath() const
        {
            return this->executablePath;
        }

        FileData* newFileData(const void* data, size_t size, std::string_view filename) const
        {
            FileData* fileData = new FileData(size, filename);
            std::memcpy(fileData->getData(), data, size);

            return fileData;
        }

        // clang-format off
        STRINGMAP_DECLARE(fileTypes, FileType,
            { "file",      FILETYPE_FILE      },
            { "directory", FILETYPE_DIRECTORY },
            { "symlink",   FILETYPE_SYMLINK   },
            { "other",     FILETYPE_OTHER     }
        );

        STRINGMAP_DECLARE(commonPaths, CommonPath,
            { "appsavedir",    COMMONPATH_APP_SAVEDIR    },
            { "appdocuments",  COMMONPATH_APP_DOCUMENTS  },
            { "userhome",      COMMONPATH_USER_HOME      },
            { "userappdata",   COMMONPATH_USER_APPDATA   },
            { "userdesktop",   COMMONPATH_USER_DESKTOP   },
            { "userdocuments", COMMONPATH_USER_DOCUMENTS }
        );

        STRINGMAP_DECLARE(mountPermissions, MountPermissions,
            { "read",      MOUNT_PERMISSIONS_READ      },
            { "readwrite", MOUNT_PERMISSIONS_READWRITE }
        );

        STRINGMAP_DECLARE(loadModes, LoadMode,
            { "b",  LOADMODE_BINARY },
            { "t",  LOADMODE_TEXT   },
            { "bt", LOADMODE_ANY    }
        );
        // clang-format on

      protected:
        std::string executablePath;
    };
} // namespace love
