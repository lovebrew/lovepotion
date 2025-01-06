#pragma once

#include "common/Data.hpp"
#include "common/Exception.hpp"
#include "common/int.hpp"

#include <memory>
#include <string>

namespace love
{
    class FileData : public Data
    {
      public:
        static Type type;

        FileData(uint64_t size, const std::string& filename);

        FileData(const FileData& other);

        virtual ~FileData();

        FileData* clone() const override;

        void* getData() const override;

        size_t getSize() const override;

        const std::string& getFilename() const;

        const std::string& getExtension() const;

        const std::string& getName() const;

      private:
        char* data;
        uint64_t size;

        std::string filename;
        std::string extension;
        std::string name;
    };
} // namespace love
