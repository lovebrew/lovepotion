#pragma once

#include "modules/filesystem/File.tcc"

#include <physfs.h>

namespace love
{
    class File : public FileBase<File>
    {
      public:
        File(std::string_view filename, Mode mode);

        virtual ~File();

        File* clone() const override;

        int64_t read(void* destination, int64_t size);

        bool write(const void* data, int64_t size);

        bool flush();

        int64_t getSize() override;

        bool seek(int64_t position, SeekOrigin origin = SEEKORIGIN_BEGIN);

        int64_t tell() override;

        using FileBase::read;

        using FileBase::write;

        bool open(Mode mode) override;

        bool close() override;

        bool isOpen() const override;

        bool isEOF();

        bool setBuffer(BufferMode mode, int64_t size);

        BufferMode getBuffer(int64_t& size) const;

        const std::string& getFilename() const;

      private:
        File(const File& other);
        PHYSFS_File* file;
    };
} // namespace love
