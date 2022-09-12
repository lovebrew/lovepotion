#pragma once

#include <common/module.hpp>
#include <common/strongreference.hpp>

#include <objects/data/filedata/filedata.hpp>
#include <objects/file/file.hpp>

#include <limits.h>
#include <vector>

namespace love
{
    class Filesystem : public Module
    {
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
            APP_SAVEDIR,
            APP_DOCUMENTS,
            USER_HOME,
            USER_APPDATA,
            USER_DOCUMENTS,
            PATH_MAX_ENUM
        };

        enum MountPermissions
        {
            MOUNT_READ,
            MOUNT_READWRITE,
            MOUNT_MAX_ENUM
        };

        struct Info
        {
            int64_t size;
            int64_t modtime;
            FileType type;
            bool readOnly;
        };

        Filesystem()
        {}

        virtual ~Filesystem()
        {}

        ModuleType GetModuleType() const override
        {
            return M_FILESYSTEM;
        }

        virtual void Init(const char* arg0) = 0;

        virtual void SetFused(bool fused) = 0;

        virtual bool IsFused() const = 0;

        virtual bool SetIdentity(const char* identity, bool appendToPath) = 0;

        virtual std::string GetIdentity() const = 0;

        virtual bool SetSource(const char* source) = 0;

        virtual std::string GetSource() const = 0;

        virtual bool Mount(const char* archive, const char* mountpoint,
                           bool appendToPath = false) = 0;

        virtual bool Mount(Data* data, const char* archive, const char* mountpoint,
                           bool appendToPath = false) = 0;

        virtual bool MountFullPath(const char* archive, const char* mountPoint,
                                   MountPermissions permissions, bool appendToPath = false) = 0;

        virtual bool MountCommonPath(CommonPath path, const char* mountPoint,
                                     MountPermissions permissions, bool appendToPath = false) = 0;

        virtual bool UnMount(const char* archive) = 0;

        virtual bool UnMount(Data* data) = 0;

        virtual bool UnMount(CommonPath path) = 0;

        virtual bool UnMountFullPath(const char* fullPath) = 0;

        virtual File* OpenFile(const char* filename, File::Mode mode) const = 0;

        FileData* NewFileData(const void* data, size_t size, const char* filename) const;

        virtual std::string GetFullCommonPath(CommonPath path) = 0;

        virtual std::string GetWorkingDirectory() = 0;

        virtual std::string GetUserDirectory() = 0;

        virtual std::string GetAppdataDirectory() = 0;

        virtual std::string GetSaveDirectory() = 0;

        virtual std::string GetSourceBaseDirectory() const = 0;

        virtual std::string GetRealDirectory(const char* filename) const = 0;

        virtual bool GetInfo(const char* filename, Info& info) const = 0;

        virtual bool CreateDirectory(const char* name) = 0;

        virtual bool Remove(const char* filename) = 0;

        virtual FileData* Read(const char* filename, int64_t size) const = 0;

        virtual FileData* Read(const char* filename) const = 0;

        virtual void Write(const char* filename, const void* data, int64_t size) const = 0;

        virtual void Append(const char* filename, const void* data, int64_t size) const = 0;

        virtual bool GetDirectoryItems(const char* directory, std::vector<std::string>& items) = 0;

        virtual void SetSymlinksEnabled(bool enable) = 0;

        virtual bool AreSymlinksEnabled() const = 0;

        virtual std::vector<std::string>& GetRequirePath() = 0;

        // virtual std::vector<std::string>& GetCRequirePath() = 0;

        virtual void AllowMountingForPath(const std::string& path) = 0;

        virtual bool SetupWriteDirectory() = 0;

        bool IsRealDirectory(const std::string& path) const;

        bool CreateRealDirectory(const std::string& path);

        std::string GetExecutablePath() const;

        static bool GetConstant(const char* in, FileType& out);
        static bool GetConstant(FileType in, const char*& out);
        static std::vector<const char*> GetConstants(FileType);

        static bool GetConstant(const char* in, CommonPath& out);
        static bool GetConstant(CommonPath in, const char*& out);
        static std::vector<const char*> GetConstants(CommonPath);

        static bool GetConstant(const char* in, MountPermissions& out);
        static bool GetConstant(MountPermissions in, const char*& out);
        static std::vector<const char*> GetConstants(MountPermissions);

      protected:
        static constexpr int64_t MAX_STAMP = 0x20000000000000;

        std::string executablePath;

        bool GetRealPathType(const std::string& path, FileType& ftype) const;
    };
} // namespace love
