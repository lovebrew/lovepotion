#pragma once

#include "common/Data.hpp"
#include "common/Object.hpp"
#include "common/Stream.hpp"
#include "common/StrongRef.hpp"
#include "common/int.hpp"

#include "modules/filesystem/FileData.hpp"

#include "common/Map.hpp"

#include <cstring>
#include <string_view>

namespace love
{
    class FileBase : public Stream
    {
      public:
        static inline Type type = Type("File", &Stream::type);

        static constexpr int64_t MAX_FILE_SIZE = 0x20000000000000LL;
        static constexpr int64_t MAX_MODTIME   = 0x20000000000000LL;

        static constexpr int64_t SIZE_ALL = -1;

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

        FileBase(const std::string& filename) :
            filename(filename),
            mode(MODE_CLOSED),
            bufferMode(BUFFER_NONE),
            bufferSize(0)
        {}

        FileBase(const FileBase& other) :
            filename(other.filename),
            mode(MODE_CLOSED),
            bufferMode(other.bufferMode),
            bufferSize(other.bufferSize)
        {}

        virtual ~FileBase()
        {}

        using Stream::read;

        using Stream::write;

        virtual bool open(Mode mode) = 0;

        virtual bool close() = 0;

        bool isReadable() const override
        {
            return getMode() == MODE_READ;
        }

        bool isWritable() const override
        {
            return getMode() == MODE_WRITE || getMode() == MODE_APPEND;
        }

        bool isSeekable() const override
        {
            return isOpen();
        }

        virtual bool isOpen() const = 0;

        FileData* read(int64_t size);

        FileData* read()
        {
            return this->read(SIZE_ALL);
        }

        Mode getMode() const
        {
            return this->mode;
        }

        virtual bool isEOF() = 0;

        virtual bool setBuffer(BufferMode bufferMode, int64_t size) = 0;

        virtual BufferMode getBuffer(int64_t& size) const = 0;

        virtual const std::string& getFilename() const = 0;

        std::string getExtension() const
        {
            std::string filename = this->getFilename();
            size_t pos           = filename.find_last_of('.');

            if (pos == std::string::npos)
                return std::string {};

            return filename.substr(pos + 1);
        }

        // clang-format off
        STRINGMAP_DECLARE(OpenModes, Mode,
            { "c", MODE_CLOSED },
            { "r", MODE_READ   },
            { "w", MODE_WRITE  },
            { "a", MODE_APPEND }
        );

        STRINGMAP_DECLARE(BufferModes, BufferMode,
            { "none", BUFFER_NONE },
            { "line", BUFFER_LINE },
            { "full", BUFFER_FULL }
        );
        // clang-format on

      protected:
        std::string filename;

        Mode mode;

        BufferMode bufferMode;
        int64_t bufferSize;
    };
} // namespace love
