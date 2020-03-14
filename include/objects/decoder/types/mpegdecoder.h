#pragma once

#include <mpg123.h>

#include "objects/decoder/decoder.h"

namespace love
{
    class MPEGDecoder : public Decoder
    {
        public:
            struct MPEGFile
            {
                unsigned char * data;
                size_t size;
                size_t offset;

                MPEGFile(Data * data) : data((unsigned char *)data->GetData()),
                                        size(data->GetSize()),
                                        offset(0)
                {}
            };

            MPEGDecoder(Data * data, int bufferSize);
            ~MPEGDecoder();

            static bool Accepts(const std::string & ext);
            static void Quit();

            Decoder * Clone();

            int Decode();

            bool Seek(double position);

            bool Rewind();

            bool IsSeekable();

            int GetChannelCount() const;

            int GetBitDepth() const;

            double GetDuration();

        private:
            MPEGFile file;

            mpg123_handle * handle;
            static bool inited;
            int channels;
            double duration;
    };
}
