#pragma once

#include "common/Data.hpp"
#include "common/Object.hpp"
#include "common/Stream.hpp"
#include "common/StrongRef.hpp"
#include "common/int.hpp"

#include "modules/filesystem/FileData.hpp"

#include "utility/map.hpp"

#include <string_view>

namespace love
{
    template<class T>
    class FileBase : public Stream
    {
      public:
        static inline Type type = Type("File", &Stream::type);

        static constexpr int64_t MAX_FILE_SIZE = 0x20000000000000LL;
        static constexpr int64_t MAX_MODTIME   = 0x20000000000000LL;

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

        FileBase(std::string_view filename) :
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

        FileData* read(int64_t size)
        {
            bool isOpen = this->isOpen();

            if (!isOpen && !this->open(MODE_READ))
                throw love::Exception("Could not read file {}.", this->getFilename());

            int64_t max     = this->getSize();
            int64_t current = this->tell();

            if (size < 0)
                throw love::Exception("Invalid read size.");

            current = std::clamp(current, (int64_t)0, max);

            if (current + size > max)
                size = max - current;

            StrongRef<FileData> data(new FileData(size, this->getFilename()), Acquire::NO_RETAIN);
            int64_t bytesRead = this->read(data->getData(), size);

            if (bytesRead < 0 || (bytesRead == 0 && bytesRead != size))
            {
                delete data;
                throw love::Exception("Could not read from file");
            }

            if (bytesRead < size)
            {
                // clang-format off
                StrongRef<FileData> temp(new FileData(bytesRead, this->getFilename()), Acquire::NO_RETAIN);
                std::copy_n((const uint8_t*)data->getData(), (size_t)bytesRead, (uint8_t*)temp->getData());
                // clang-format on

                data = temp;
            }

            if (!isOpen)
                this->close();

            data->retain();
            return data;
        }

        FileData* read()
        {
            return this->read(this->getSize());
        }

        Mode getMode() const
        {
            return this->mode;
        }

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
        STRINGMAP_DECLARE(openModes, Mode,
            { "c", MODE_CLOSED },
            { "r", MODE_READ   },
            { "w", MODE_WRITE  },
            { "a", MODE_APPEND }
        );

        STRINGMAP_DECLARE(bufferModes, BufferMode,
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
