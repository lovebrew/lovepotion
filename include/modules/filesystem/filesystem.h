/*
** modules/filesystem.h
** @brief : Filesystem operations (read/write files)
*/

#pragma once

#include "common/module.h"

#include "objects/file/file.h"
#include "objects/filedata/filedata.h"

#include <vector>

#define MAX_STAMP 0x20000000000000LL

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

        struct Info
        {
            int64_t size;
            int64_t modtime;
            FileType type;
        };

        Filesystem();
        ~Filesystem();

        static love::Type type;

        /* Module implementors */

        const char* GetName() const override
        {
            return "love.filesystem";
        }

        ModuleType GetModuleType() const
        {
            return M_FILESYSTEM;
        }

        /* Löve2D Functions */

        void Init(const char* arg0);

        void Append(const char* filename, const void* data, int64_t size);

        bool CreateDirectory(const char* name);

        void GetDirectoryItems(const char* directory, std::vector<std::string>& items);

        const char* GetIdentity();

        bool GetInfo(const char* filename, Info& info) const;

        std::string GetSaveDirectory();

        File* NewFile(const char* filename);

        FileData* NewFileData(const void* data, size_t size, const char* filename);

        FileData* Read(const char* filename, int64_t size = File::ALL);

        bool Remove(const char* filename);

        bool SetIdentity(const char* identity, bool appendToPath);

        void Write(const char* filename, const void* data, int64_t size);

        void AllowMountingForPath(const std::string& path);

        bool Mount(const char* archive, const char* mountpoint, bool appendToPath = false);

        bool Mount(Data* data, const char* archive, const char* mountpoint,
                   bool appendToPath = false);

        bool UnMount(const char* archive);

        bool UnMount(Data* data);

        bool SetSource(const char* source);

        void SetFused(bool fused);

        bool IsFused() const;

        const char* GetSource() const;

        const char* GetWorkingDirectory();

        std::string GetExecutablePath() const;

        std::string GetSourceBaseDirectory() const;

        bool SetupWriteDirectory();

        std::vector<std::string>& GetRequirePath();

        void SetSymLinksEnabled(bool enable);

        std::string GetUserDirectory();

        std::string GetRealDirectory(const char* filename) const;

        // std::vector<std::string> & GetCRequirePath() override;

        /* Helper Functions */

        static bool GetConstant(const char* in, FileType& out);
        static bool GetConstant(FileType in, const char*& out);

        static std::vector<const char*> GetConstants(FileType);

      private:
        std::string identity;
        std::string relativeSavePath;
        std::string fullSavePath;

        std::string appdata;
        std::string gameSource;
        std::string cwd;
        std::string exePath;

        std::vector<std::string> allowedMountPaths;

        std::vector<std::string> requirePath;
        std::vector<std::string> cRequirePath;

        std::map<std::string, StrongReference<Data>> mountedData;

        bool fused;
        bool fusedSet;

        std::string GetAppDataDirectory();
    };
} // namespace love
