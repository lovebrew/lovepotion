#pragma once

#include "objects/decoder/decoder.h"
#include <libmodplug/modplug.h>

namespace love
{
    class ModPlugDecoder : public Decoder
    {
      public:
        ModPlugDecoder(Data* data, int bufferSize);

        virtual ~ModPlugDecoder();

        static bool Accepts(const std::string& ext);

        Decoder* Clone();

        int Decode();

        int Decode(s16* buffer);

        bool Seek(double position);

        bool Rewind();

        bool IsSeekable();

        int GetChannelCount() const;

        int GetBitDepth() const;

        double GetDuration();

      private:
        ModPlugFile* plug;
        ModPlug_Settings settings;

        double duration;
    };
} // namespace love
