#pragma once

#include <functional>

#include "objects/file/file.h"

#include <ogg/ogg.h>

namespace love
{
    class OggDemuxer
    {
      public:
        OggDemuxer(File* file);

        ~OggDemuxer();

        enum StreamType
        {
            TYPE_THEORA,
            TYPE_UNKNOWN
        };

        static constexpr int SYNC_VALUE          = 0x2000;
        static constexpr int THEORA_BYTES_MIN    = 0x07;
        static constexpr int THEORA_HEADER_TYPE  = 0x80;
        static constexpr double REWIND_THRESHOLD = 0.01;

        StreamType FindStream();

        bool ReadPacket(ogg_packet& packet, bool mustSucceed = false);

        void ReSync();

        bool IsEOS() const;

        const std::string& GetFilename() const;

        bool Seek(ogg_packet& packet, double target, std::function<double(int64_t)> getTime);

      private:
        StrongReference<File> file;

        ogg_sync_state sync;
        ogg_stream_state stream;
        ogg_page page;

        bool intiialized;
        int serial;
        bool endOfStream;

        bool ReadPage(bool errorEOF = false);

        StreamType DetermineType();
    };
} // namespace love
