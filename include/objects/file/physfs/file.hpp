#pragma once

#include <objects/file/file.hpp>

#include <physfs.h>

#include <string>

namespace love::physfs
{
    class File : public love::File
    {
      public:
        File(const std::string& filename, Mode mode);

        virtual ~File();

        File* Clone() override;

        int64_t Read(void* destination, int64_t size) override;

        bool Write(const void* data, int64_t size) override;

        bool Flush() override;

        int64_t GetSize() override;

        bool Seek(int64_t position, SeekOrigin origin) override;

        int64_t Tell() override;

        using love::File::Read;

        using love::File::Write;

        bool Open(Mode mode) override;

        bool Close() override;

        bool IsOpen() const override;

        bool IsEOF() override;

        bool SetBuffer(BufferMode mode, int64_t size) override;

        BufferMode GetBuffer(int64_t& size) const override;

        Mode GetMode() const override;

        const std::string& GetFilename() const override;

      private:
        File(const File& other);

        std::string filename;
        PHYSFS_File* file;

        Mode fileMode;

        BufferMode bufferMode;
        int64_t bufferSize;
    };
} // namespace love::physfs
