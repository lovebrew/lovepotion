#pragma once

#include "objects/decoder/decoder.h"
#include "dr/dr_flac.h"

namespace love
{
    class FLACDecoder : public Decoder
    {
        public:
            FLACDecoder(Data * data, int bufferSize);
            ~FLACDecoder();

            static bool Accepts(const std::string & ext);

            Decoder * Clone();

            int Decode();

            int Decode(s16 * buffer);

            bool Seek(double position);

            bool Rewind();

            bool IsSeekable();

            int GetChannelCount() const;

            int GetBitDepth() const;

            int GetSampleRate() const;

            double GetDuration();

        private:
            drflac * flac;
    };
}
