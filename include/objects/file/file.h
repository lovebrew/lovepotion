#pragma once

#include "objects/object.h"

#include "common/exception.h"
#include "objects/filedata/filedata.h"

#include <physfs.h>

#include <vector>

namespace love
{
    class File : public Object
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

        static const int64_t ALL = -1;

        File(const std::string& filename);

        virtual ~File();

        bool Close();

        bool Flush();

        BufferMode GetBuffer(int64_t& size) const;

        const std::string& GetFilename() const;

        Mode GetMode();

        int64_t GetSize();

        bool IsEOF();

        bool IsOpen();

        bool Open(File::Mode mode);

        int64_t Read(void* destination, int64_t size);
        FileData* Read(int64_t size = ALL);

        bool Seek(uint64_t position);

        bool SetBuffer(BufferMode mode, int64_t size);

        int64_t Tell();

        bool Write(const void* data, int64_t size);
        bool Write(Data* data, int64_t size);

        /* OPEN MODES */

        static bool GetConstant(const char* in, Mode& out);
        static bool GetConstant(Mode in, const char*& out);

        static std::vector<const char*> GetConstants(Mode mode);

        /* BUFFER MODES */

        static bool GetConstant(const char* in, BufferMode& out);
        static bool GetConstant(BufferMode in, const char*& out);

        static std::vector<const char*> GetConstants(BufferMode mode);

      private:
        std::string filename;

        PHYSFS_file* file;
        Mode mode;

        BufferMode bufferMode;
        int64_t bufferSize;
    };
} // namespace love
