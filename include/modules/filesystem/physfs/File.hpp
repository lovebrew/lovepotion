#pragma once

#include "modules/filesystem/File.tcc"

#include <physfs.h>

namespace love
{
    class File : public FileBase
    {
      public:
        File(const std::string& filename, Mode mode);

        virtual ~File();

        File* clone() const override;

        int64_t read(void* destination, int64_t size);

        bool write(const void* data, int64_t size);

        bool flush();

        int64_t getSize() override;

        virtual bool seek(int64_t position, SeekOrigin origin = SEEKORIGIN_BEGIN) override;

        int64_t tell() override;

        using FileBase::read;

        using FileBase::write;

        bool open(Mode mode) override;

        bool close() override;

        bool isOpen() const override;

        bool isEOF() override;

        bool setBuffer(BufferMode mode, int64_t size) override;

        BufferMode getBuffer(int64_t& size) const override;

        const std::string& getFilename() const override;

      private:
        File(const File& other);
        PHYSFS_File* file;
    };
} // namespace love
