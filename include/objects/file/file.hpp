#pragma once

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/stream/stream.hpp>

#include <objects/data/filedata/filedata.hpp>

#include <vector>

namespace love
{
    class File : public Stream
    {
      public:
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

        static Type type;

        virtual ~File()
        {}

        bool IsReadable() const override
        {
            return this->GetMode() == MODE_READ;
        }

        bool IsWritable() const override
        {
            return this->GetMode() == MODE_WRITE || this->GetMode() == MODE_APPEND;
        }

        bool IsSeekable() const override
        {
            return this->IsOpen();
        }

        using Stream::Read;

        using Stream::Write;

        virtual bool Open(Mode mode) = 0;

        virtual bool Close() = 0;

        virtual bool IsOpen() const = 0;

        FileData* Read(int64_t size) override;

        FileData* Read();

        virtual bool IsEOF() = 0;

        virtual bool SetBuffer(BufferMode mode, int64_t size) = 0;

        virtual BufferMode GetBuffer(int64_t& size) const = 0;

        virtual Mode GetMode() const = 0;

        virtual const std::string& GetFilename() const = 0;

        virtual std::string GetExtension() const;

        // clang-format off
        static constexpr BidirectionalMap modes = {
            "r", File::Mode::MODE_READ,
            "w", File::Mode::MODE_WRITE,
            "a", File::Mode::MODE_APPEND,
            "c", File::Mode::MODE_CLOSED
        };

        static constexpr BidirectionalMap bufferModes = {
            "none", File::BufferMode::BUFFER_NONE,
            "line", File::BufferMode::BUFFER_LINE,
            "full", File::BufferMode::BUFFER_FULL
        };
        // clang-format on
    };
} // namespace love
