#pragma once

#include "OggDemuxer.hpp"
#include "modules/video/VideoStream.hpp"

#include "common/int.hpp"
#include "modules/filesystem/File.tcc"
#include "modules/video/theora/OggDemuxer.hpp"

#include <ogg/ogg.h>
#include <theora/codec.h>
#include <theora/theoradec.h>

#include <thread>

namespace love
{
    class TheoraVideoStream : public VideoStream
    {
      public:
        TheoraVideoStream(FileBase* file);
        ~TheoraVideoStream();

        const void* getFrontBuffer() const;
        size_t getSize() const;
        void fillBackBuffer();
        bool swapBuffers();

        int getWidth() const;
        int getHeight() const;
        const std::string& getFilename() const;
        void setSync(FrameSync* sync);

        bool isPlaying() const;

        void threadedFillBackBuffer(double dt);

      private:
        OggDemuxer demuxer;
        bool headerParsed;
        ogg_packet packet;

        th_info info;
        th_dec_ctx* decoder;

        Frame* frontBuffer;
        Frame* backBuffer;

        unsigned int yPlaneXOffset;
        unsigned int cPlaneXOffset;
        unsigned int yPlaneYOffset;
        unsigned int cPlaneYOffset;

        std::mutex mutex;
        bool frameReady;

        double lastFrame;
        double nextFrame;

        void parseHeader();
        void seekDecoder(double target);
    };
} // namespace love
