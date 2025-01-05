#pragma once

#include "modules/filesystem/File.tcc"

namespace love
{
    class NativeFile : public FileBase
    {
      public:
        NativeFile(const std::string& filename, Mode mode);

        virtual ~NativeFile();

        NativeFile* clone() const override;

        int64_t read(void* destination, int64_t size) override;

        bool write(const void* data, int64_t size) override;

        bool flush() override;

        int64_t getSize() override;

        int64_t tell() override;

        bool seek(int64_t position, SeekOrigin origin) override;

        using FileBase::read;
        using FileBase::write;

        bool open(Mode mode) override;

        bool close() override;

        bool isOpen() const override;

        bool isEOF() override;

        bool setBuffer(BufferMode bufferMode, int64_t size) override;

        BufferMode getBuffer(int64_t& size) const override;

        const std::string& getFilename() const override;

      private:
        NativeFile(const NativeFile& other);
        static const char* getModeString(Mode mode);

        FILE* file;
    };
} // namespace love
