/*
** modules/filesystem.h
** @brief : Filesystem operations (read/write files)
*/

#pragma once

#include "objects/file/file.h"
#include "objects/filedata/filedata.h"

#define MAX_STAMP 0x20000000000000LL

namespace love
{
    class Filesystem : public Module
    {
        public:
            enum FileType
            {
                FILE,
                DIRECTORY,
                SYMLINK,
                OTHER,
                MAX_ENUM
            };

            struct Info
            {
                int64_t size;
                int64_t modtime;
                FileType type;
            };

            Filesystem();

            static love::Type type;

            /* Module implementors */

            const char * GetName() const override { return "love.filesystem"; }

            ModuleType GetModuleType() const { return M_FILESYSTEM; }

            /* Löve2D Functions */

            void Append(const char * filename, const void * data, int64_t size);

            void CreateDirectory(const char * name);

            void GetDirectoryItems(const char * directory, std::vector<std::string> & items);

            std::string GetIdentity();

            bool GetInfo(const char * filename, Info & info) const;

            std::string GetSaveDirectory();

            File * NewFile(const char * filename);

            FileData * NewFileData(const void * data, size_t size, const char * filename);

            FileData * Read(const char * filename, int64_t size = File::ALL);

            bool Remove(const char * filename);

            void SetIdentity(const char * identity);

            void Write(const char * filename, const void * data, int64_t size);

            /* Helper Functions */

            static inline bool GetConstant(const char * find, FileType & type) {
                if (types.find(find) != types.end())
                {
                    type = types[find];
                    return true;
                }

                return false;
            }

            static inline bool GetConstant(FileType find, const char *& out) {
                for (auto it = types.begin(); it != types.end(); it++)
                {
                    if (it->second == find)
                    {
                        out = it->first;
                        return true;
                    }
                }

                return false;
            }

        private:
            std::string identity;

            static inline std::map<const char *, FileType> types = {
                { "file",      FileType::FILE },
                { "directory", FileType::DIRECTORY },
                { "symlink",   FileType::SYMLINK },
                { "other",     FileType::OTHER }
            };

            // Löve2D Functions

            std::string Redirect(const char * path);
    };
}
