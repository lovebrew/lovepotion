#pragma once

#include <map>

#include <modules/filesystem/filesystem.hpp>

#include <objects/file/physfs/file.hpp>

#include <array>
#include <string>

namespace love::physfs
{
    class Filesystem : public love::Filesystem
    {
      public:
        Filesystem();

        virtual ~Filesystem();

        const char* GetName() const override
        {
            return "love.filesystem.physfs";
        }

        void Init(const char* arg0) override;

        void SetFused(bool fused) override;

        bool IsFused() const override;

        bool SetIdentity(const char* identity, bool appendToPath = false) override;

        std::string GetIdentity() const override;

        bool SetSource(const char* source) override;

        std::string GetSource() const override;

        bool Mount(const char* archive, const char* mountPoint, bool appendToPath = false) override;

        bool Mount(Data* data, const char* name, const char* mountPoint,
                   bool appendToPath = false) override;

        bool MountFullPath(const char* archive, const char* mountPoint,
                           MountPermissions permissions, bool appendToPath = false) override;

        bool MountCommonPath(CommonPath path, const char* mountPoint, MountPermissions permissions,
                             bool appendToPath = false) override;

        bool UnMount(const char* archive) override;

        bool UnMount(Data* data) override;

        bool UnMount(CommonPath path) override;

        bool UnMountFullPath(const char* fullPath) override;

        love::File* OpenFile(const char* filename, File::Mode mode) const override;

        std::string GetFullCommonPath(CommonPath path) override;

        std::string GetWorkingDirectory() override;

        std::string GetUserDirectory() override;

        std::string GetAppdataDirectory() override;

        std::string GetSaveDirectory() override;

        std::string GetSourceBaseDirectory() const override;

        std::string GetRealDirectory(const char* filename) const override;

        bool GetInfo(const char* filepath, Info& info) const override;

        bool CreateDirectory(const char* directory) override;

        bool Remove(const char* file) override;

        FileData* Read(const char* filename, int64_t size) const override;

        FileData* Read(const char* filename) const override;

        void Write(const char* filename, const void* data, int64_t size) const override;

        void Append(const char* filename, const void* data, int64_t size) const override;

        bool GetDirectoryItems(const char* directory, std::vector<std::string>& items) override;

        void SetSymlinksEnabled(bool enable) override;

        bool AreSymlinksEnabled() const override;

        std::vector<std::string>& GetRequirePath() override;

        // std::vector<std::string>& GetCRequirePath() override;

        void AllowMountingForPath(const std::string& path) override;

        bool SetupWriteDirectory() override;

        static const char* GetLastError(bool clear = false);

      private:
        struct CommonPathMountInfo
        {
            bool mounted;
            std::string mountPoint;
            MountPermissions permissions;
        };

        bool MountCommonPathInternal(CommonPath path, const char* mountpoint,
                                     MountPermissions permissions, bool appendToPath,
                                     bool createDirectory);

        static bool IsAppCommonPath(CommonPath path);

        std::string currentDirectory;
        std::string saveIdentity;

        bool appendIdentityToPath;

        std::string gameSource;

        bool fused;
        bool fusedSet;

        std::vector<std::string> requirePath;
        // std::vector<std::string> cRequirePath

        std::vector<std::string> allowedMountPaths;

        std::map<std::string, StrongReference<Data>> mountedData;

        std::array<std::string, CommonPath::PATH_MAX_ENUM> fullPaths;
        std::array<CommonPathMountInfo, CommonPath::PATH_MAX_ENUM> commonPathMountInfo;

        bool saveDirectoryNeedsMounting;

        std::array<CommonPath, 0x02> appCommonPaths;
    };
} // namespace love::physfs
