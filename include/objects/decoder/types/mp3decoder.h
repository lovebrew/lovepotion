#pragma once

#include <mpg123.h>

#include "objects/decoder/decoder.h"

namespace love
{
    class MP3Decoder : public Decoder
    {
      public:
        struct MP3File
        {
            unsigned char* data;
            size_t size;
            size_t offset;

            MP3File(Data* data) :
                data((unsigned char*)data->GetData()),
                size(data->GetSize()),
                offset(0)
            {}
        };

        MP3Decoder(Data* data, int bufferSize);
        ~MP3Decoder();

        static bool Accepts(const std::string& ext);

        static void Quit();

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
        MP3File file;

        mpg123_handle* handle;
        static bool inited;
        int channels;
        double duration;
    };
} // namespace love
