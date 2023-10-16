#pragma once

#include <common/data.hpp>

#include <memory>

namespace love
{
    class FileData : public Data
    {
      public:
        static Type type;

        FileData(uint64_t size, const std::string& filename);

        FileData(const FileData& content);

        FileData* Clone() const;

        void* GetData() const;

        size_t GetSize() const;

        const std::string& GetFilename() const;

        const std::string& GetExtension() const;

        const std::string& GetName() const;

      private:
        std::unique_ptr<char[]> data;
        uint64_t size;

        std::string filename;
        std::string extension;
        std::string name;
    };
} // namespace love
