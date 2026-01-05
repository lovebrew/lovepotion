#pragma once

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
    class TheoraVideoStream final : public VideoStream
    {
      public:
        TheoraVideoStream(FileBase* file);

        virtual ~TheoraVideoStream();

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

        th_info videoInfo;
        th_dec_ctx* decoder;

        Frame* frontBuffer;
        Frame* backBuffer;

        uint32_t yPlaneXOffset;
        uint32_t cPlaneXOffset;
        uint32_t yPlaneYOffset;
        uint32_t cPlaneYOffset;

        std::recursive_mutex bufferMutex;
        bool frameReady;

        double lastFrame;
        double nextFrame;

        void parseHeader();

        void seekDecoder(double target);

#if defined(__3DS__)
        Handle handle;
#endif
    };
} // namespace love
