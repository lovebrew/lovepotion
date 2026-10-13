#include "modules/video/theora/TheoraVideoStream.hpp"
#include "common/Exception.hpp"
#include "modules/video/theora/OggDemuxer.hpp"
#include <cstdint>
#include <mutex>
#include <ogg/config_types.h>
#include <theora/codec.h>
#include <theora/theoradec.h>

namespace love
{
    TheoraVideoStream::TheoraVideoStream(FileBase* file) :
        demuxer(file),
        headerParsed(false),
        decoder(nullptr),
        frameReady(false),
        lastFrame(0),
        nextFrame(0)
    {
        if (demuxer.findStream() != OggDemuxer::TYPE_THEORA)
            throw love::Exception("Invalid video file, video is not theora.");

        th_info_init(&this->info);

        this->frontBuffer = new Frame();
        this->backBuffer  = new Frame();

        try
        {
            this->parseHeader();
        }
        catch (love::Exception& e)
        {
            delete this->backBuffer;
            delete this->frontBuffer;
            th_info_clear(&this->info);
            throw e;
        }

        this->sync.set(new DeltaSync(), Acquire::NO_RETAIN);
    }

    TheoraVideoStream::~TheoraVideoStream()
    {
        if (this->decoder)
            th_decode_free(this->decoder);

        th_info_clear(&this->info);

        delete this->frontBuffer;
        delete this->backBuffer;
    }

    int TheoraVideoStream::getWidth() const
    {
        if (this->headerParsed)
            return this->info.pic_width;

        return 0;
    }

    int TheoraVideoStream::getHeight() const
    {
        if (this->headerParsed)
            return this->info.pic_height;

        return 0;
    }

    const std::string& TheoraVideoStream::getFilename() const
    {
        return this->demuxer.getFilename();
    }

    void TheoraVideoStream::setSync(FrameSync* sync)
    {
        std::unique_lock lock(this->mutex);
        this->sync = sync;
    }

    const void* TheoraVideoStream::getFrontBuffer() const
    {
        return this->frontBuffer;
    }

    size_t TheoraVideoStream::getSize() const
    {
        return sizeof(Frame);
    }

    bool TheoraVideoStream::isPlaying() const
    {
        return this->sync->isPlaying() && !this->demuxer.isEos();
    }

    template<typename T>
    inline void scaleFormat(th_pixel_fmt format, T& x, T& y)
    {
        switch (format)
        {
            case TH_PF_420:
                y /= 2;
            case TH_PF_422:
                x /= 2;
                break;
            default:
                break;
        }
    }

    void TheoraVideoStream::parseHeader()
    {
        if (this->headerParsed)
            return;

        th_comment comment {};
        th_setup_info* setup = nullptr;
        th_comment_init(&comment);

        int result;
        demuxer.readPacket(this->packet);
        result = th_decode_headerin(&this->info, &comment, &setup, &this->packet);

        if (result < 0)
        {
            th_comment_clear(&comment);
            throw love::Exception("Could not find theora header.");
        }

        while (result > 0)
        {
            demuxer.readPacket(packet);
            result = th_decode_headerin(&this->info, &comment, &setup, &this->packet);
        }

        th_comment_clear(&comment);

        this->decoder = th_decode_alloc(&this->info, setup);
        th_setup_free(setup);

        Frame* buffers[2] = { backBuffer, frontBuffer };

        this->yPlaneXOffset = cPlaneXOffset = this->info.pic_x;
        this->yPlaneYOffset = cPlaneYOffset = this->info.pic_y;
        scaleFormat(this->info.pixel_fmt, cPlaneXOffset, cPlaneYOffset);

#if !defined(__3DS__)
        // fill buffer data
#endif

        this->headerParsed = true;
        th_decode_packetin(this->decoder, &this->packet, nullptr);
    }

    void TheoraVideoStream::seekDecoder(double target)
    {
        bool success = this->demuxer.seek(this->packet, target, [this](int64_t position) {
            return th_granule_time(this->decoder, position);
        });

        if (!success)
            return;

        this->lastFrame = this->nextFrame = -1;
        th_decode_ctl(this->decoder, TH_DECCTL_SET_GRANPOS, &packet.granulepos, sizeof(packet.granulepos));
    }

    void TheoraVideoStream::threadedFillBackBuffer(double dt)
    {
        this->sync->update(dt);
        double position = this->sync->getPosition();

        if (position < this->lastFrame)
            this->seekDecoder(position);

        th_ycbcr_buffer bufferInfo {};
        bool hasFrame = false;

        unsigned int framesBehind = 0;
        bool failedSeek           = false;

        while (!this->demuxer.isEos() && position >= this->nextFrame)
        {
            if (framesBehind++ > 5 && !failedSeek)
            {
                this->seekDecoder(position);
                framesBehind = 0;
                failedSeek   = true;
            }

            th_decode_ycbcr_out(this->decoder, bufferInfo);
            hasFrame = true;

            ogg_int64_t decoderPosition;
            do
            {
                if (this->demuxer.readPacket(packet))
                    return;

                // clang-format off
                if (packet.granulepos > 0)
                    th_decode_ctl(this->decoder, TH_DECCTL_SET_GRANPOS, &packet.granulepos, sizeof(packet.granulepos));
                // clang-format on
            } while (th_decode_packetin(this->decoder, &packet, &decoderPosition) != 0);

            this->lastFrame = this->nextFrame;
            this->nextFrame = th_granule_time(this->decoder, decoderPosition);
        }

        if (hasFrame)
        {
            {
                std::unique_lock lock(this->mutex);
                frameReady = false;
            }

#if !defined(__3DS__)
            // fill backbuffer
#endif

            {
                std::unique_lock lock(this->mutex);
                frameReady = true;
            }
        }
    }

    void TheoraVideoStream::fillBackBuffer()
    {}

    bool TheoraVideoStream::swapBuffers()
    {
        if (this->demuxer.isEos())
            return false;

        if (!this->sync->isPlaying())
            return false;

        std::unique_lock lock(this->mutex);
        if (!this->frameReady)
            return false;

        Frame* temp       = this->frontBuffer;
        this->frontBuffer = this->backBuffer;
        this->backBuffer  = temp;
        return true;
    }
} // namespace love
