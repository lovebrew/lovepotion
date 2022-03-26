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

        virtual const void* GetFrontBuffer() const = 0;

        virtual size_t GetSize() const = 0;

        void FillBackBuffer();

        virtual bool SwapBuffers() = 0;

        int GetWidth() const;

        int GetHeight() const;

        const std::string& GetFilename() const;

        void SetSync(FrameSync* other);

        bool IsPlaying() const;

        virtual void ThreadedFillBackBuffer(double dt) = 0;

        virtual void DeleteBuffers() = 0;

      protected:
        OggDemuxer demuxer;
        bool headerParsed;

        ogg_packet packet;

        th_info info;
        th_dec_ctx* decoder;

        thread::MutexRef bufferMutex;
        bool frameReady;

        double lastFrame;
        double nextFrame;

        virtual void ParseHeader() = 0;

        void SeekDecoder(double target);
    }
} // namespace love::common
