#pragma once

#include "modules/filesystem/File.tcc"

#include <functional>
#include <ogg/ogg.h>

namespace love
{
    class OggDemuxer
    {
      public:
        enum StreamType
        {
            TYPE_THEORA,
            TYPE_UNKNOWN
        };

        static constexpr double REWIND_THRESHOLD = 0.01;
        static constexpr int DEFAULT_BUFFER_SIZE = 4096;

        OggDemuxer(FileBase* file);

        ~OggDemuxer();

        StreamType findStream();

        bool readPacket(ogg_packet& packet, bool mustSucceed = false);

        void resync();

        bool isEos() const;

        const std::string& getFilename() const;

        bool seek(ogg_packet& packet, double target, std::function<double(int64_t)> getTime);

      private:
        StrongRef<FileBase> file;

        ogg_sync_state sync;
        ogg_stream_state stream;
        ogg_page page;

        bool streamInited;
        int videoSerial;
        bool eos;

        bool readPage(bool errorEof = false);

        StreamType determineType();
    };
} // namespace love
