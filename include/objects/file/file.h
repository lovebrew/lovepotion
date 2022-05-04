#pragma once

#include "objects/object.h"

#include "common/exception.h"
#include "objects/filedata/filedata.h"

#include "objects/stream/stream.h"

#include <physfs.h>

#include <vector>

namespace love
{
    class File : public Stream
    {
      public:
        static love::Type type;

        enum Mode
        {
            MODE_CLOSED,
            MODE_READ,
            MODE_WRITE,
            MODE_APPEND,
            MODE_MAX_ENUM
        };

        enum BufferMode
        {
            BUFFER_NONE,
            BUFFER_LINE,
            BUFFER_FULL,
            BUFFER_MAX_ENUM
        };

        File(const std::string& filename, Mode mode);

        virtual ~File();

        bool IsReadable() const override;

        bool IsWritable() const override;

        bool IsSeekable() const override;

        File* Clone() override;

        int64_t Read(void* destination, int64_t size) override;

        bool Write(const void* data, int64_t size) override;

        bool Flush() override;

        int64_t GetSize() override;

        bool Seek(int64_t position, SeekOrigin origin = SeekOrigin::SEEK_ORIGIN_BEGIN) override;

        int64_t Tell() override;

        FileData* Read(int64_t size) override;

        FileData* Read();

        bool Open(File::Mode mode);

        bool Close();

        bool IsOpen() const;

        bool IsEOF();

        bool SetBuffer(BufferMode mode, int64_t size);

        BufferMode GetBuffer(int64_t& size) const;

        const std::string& GetFilename() const;

        Mode GetMode();

        using Stream::Write;

        using Stream::Read;

        /* OPEN MODES */

        static bool GetConstant(const char* in, Mode& out);
        static bool GetConstant(Mode in, const char*& out);

        static std::vector<const char*> GetConstants(Mode mode);

        /* BUFFER MODES */

        static bool GetConstant(const char* in, BufferMode& out);
        static bool GetConstant(BufferMode in, const char*& out);

        static std::vector<const char*> GetConstants(BufferMode mode);

      private:
        File(const File& other);

        std::string filename;

        PHYSFS_file* file;
        Mode mode;

        BufferMode bufferMode;
        int64_t bufferSize;
    };
} // namespace love
