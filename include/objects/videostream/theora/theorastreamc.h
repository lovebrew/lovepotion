#pragma once

#include <ogg/ogg.h>
#include <theora/codec.h>
#include <theora/theoradec.h>

#include "modules/thread/types/mutex.h"

#include "objects/file/file.h"
#include "objects/videostream/theora/oggdemuxer.h"
#include "objects/videostream/videostream.h"

namespace love::common
{
    class TheoraStream : public VideoStream
    {
      public:
        TheoraStream(File* file);

        ~TheoraStream();

        const void* GetFrontBuffer() const;

        virtual size_t GetSize() const = 0;

        void FillBackBuffer();

        bool SwapBuffers();

        int GetWidth() const;

        int GetHeight() const;

        const std::string& GetFilename() const;

        void SetSync(FrameSync* other);

        bool IsPlaying() const;

        void ThreadedFillBackBuffer(double dt);

        virtual void SetupBuffers() = 0;

        virtual void FillBufferData(th_ycbcr_buffer bufferInfo) = 0;

      protected:
        IFrame* frontBuffer;
        IFrame* backBuffer;

        OggDemuxer demuxer;
        bool headerParsed;

        ogg_packet packet;

        th_info info;
        th_dec_ctx* decoder;

        thread::MutexRef bufferMutex;
        bool frameReady;

        double lastFrame;
        double nextFrame;

        struct PostProcess
        {
            int current;
            int maximum;
            int offset;
        } quality;

        void ParseHeader();

        void SeekDecoder(double target);
    };
} // namespace love::common
