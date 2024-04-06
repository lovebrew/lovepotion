#pragma once

#include "common/Stream.hpp"
#include "modules/sound/Decoder.hpp"

#include "dr_mp3.h"

#include <vector>

namespace love
{
    class MP3Decoder : public Decoder
    {
      public:
        MP3Decoder(Stream* stream, int bufferSize);

        virtual ~MP3Decoder();

        Decoder* clone() override;

        virtual int decode() override;

        virtual bool seek(double position) override;

        virtual bool rewind() override;

        virtual bool isSeekable() override;

        virtual int getChannelCount() const override;

        virtual int getBitDepth() const override;

        virtual double getDuration() override;

      private:
        static size_t onRead(void* userData, void* bufferOut, size_t bytesToRead);

        static drmp3_bool32 onSeek(void* userData, int offset, drmp3_seek_origin origin);

        drmp3 handle;

        std::vector<drmp3_seek_point> seekTable;

        int64_t offset;

        double duration;
    };
} // namespace love
