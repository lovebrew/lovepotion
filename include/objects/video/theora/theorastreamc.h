#pragma once

#include <ogg/ogg.h>
#include <theora/codec.h>
#include <theora/theoradec.h>

#include "modules/thread/types/mutex.h"
#include "objects/file/file.h"
#include "objects/video/theora/oggdemuxer.h"
#include "objects/video/videostream.h"

namespace love::common
{
    class TheoraStream : public VideoStream
    {
      public:
        TheoraStream(File* file);

        ~TheoraStream();

        const void* GetFrontBuffer() const;

        size_t GetSize() const;

        void FillBackBuffer();

        bool SwapBuffers();

        int GetWidth() const;

        int GetHeight() const;

        const std::string& GetFilename() const;

        void SetSync(FrameSync* other);

        bool IsPlaying() const;

        void ThreadedFillBackBuffer(double dt);

      private:
        OggDemuxer demuxer;
        bool headerParsed;

        ogg_packet packet;

        th_info info;
        th_dec_ctx* decoder;

        thread::MutexRef bufferMutex;
        bool frameReady;

        double lastFrame;
        double nextFrame;

        void ParseHeader();
        void SeekDecoder(double target);
    }
} // namespace love::common
