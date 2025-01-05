#pragma once

#include "modules/filesystem/Filesystem.tcc"
#include "modules/filesystem/NativeFile.hpp"
#include "modules/filesystem/physfs/File.hpp"

#include <map>

namespace love
{
    class Filesystem final : public FilesystemBase
    {
      public:
        static const char* getLastError();

      public:
        Filesystem();

        virtual ~Filesystem();

        void init(const char* arg0);

        void setFused(bool fused);

        bool isFused() const;

        bool setupWriteDirectory();

        bool setIdentity(const char* identity, bool appendToPath = false);

        std::string getIdentity() const;

        bool setSource(const char* path);

        std::string getSource() const;

        bool mount(const char* archive, const char* mountpoint, bool appendToPath = false);

        // clang-format off
        bool mount(Data* data, const char* archive, const char* mountpoint, bool appendToPath = false);

        bool mountFullPath(const char* archive, const char* mountpoint, MountPermissions permissions, bool appendToPath);

        bool mountCommonPath(CommonPath path, const char* mountPoint, MountPermissions permissions, bool appendToPath);
        // clang-format on

        bool unmount(const char* archive);

        bool unmount(Data* data);

        bool unmount(CommonPath path);

        bool unmountFullPath(const char* fullpath);

        FileBase* openFile(const std::string& filename, File::Mode mode) const;

        FileBase* openNativeFile(const std::string& filepath, File::Mode mode) const;

        std::string getFullCommonPath(CommonPath path);

        std::string getWorkingDirectory();

        std::string getUserDirectory();

        std::string getAppdataDirectory();

        std::string getSaveDirectory();

        std::string getSourceBaseDirectory();

        std::string getRealDirectory(const char* filename) const;

        bool exists(const char* filepath) const;

        bool getInfo(const char* filepath, Info& info) const;

        bool createDirectory(const char* path);

        bool remove(const char* filepath);

        FileData* read(const std::string& filename, int64_t size) const;

        FileData* read(const std::string& filename) const;

        void write(const std::string& filename, const void* data, int64_t size) const;

        void append(const std::string& filename, const void* data, int64_t size) const;

        bool getDirectoryItems(const char*, std::vector<std::string>& items);

        void setSymlinksEnabled(bool enable);

        bool areSymlinksEnabled() const;

        std::vector<std::string>& getRequirePath();

        std::vector<std::string>& getCRequirePath();

      private:
        struct CommonPathMountInfo
        {
            bool mounted;
            std::string mountPoint;
            MountPermissions permissions;
        };

        bool mountCommonPathInternal(CommonPath path, const char* mountPoint, MountPermissions permissions,
                                     bool appendToPath, bool createDirectory);

        std::string currentDirectory;

        std::string saveIdentity;
        bool appendIdentityToPath;

        std::string source;

        bool fused;
        bool fusedSet;

        std::vector<std::string> requirePath;
        std::vector<std::string> cRequirePath;

        std::vector<std::string> allowedPaths;

        std::map<std::string, StrongRef<Data>> mountedData;

        std::array<std::string, COMMONPATH_MAX_ENUM> fullPaths;
        std::array<CommonPathMountInfo, COMMONPATH_MAX_ENUM> commonPathMountInfo;
        bool saveDirectoryNeedsMounting;
    };
} // namespace love
