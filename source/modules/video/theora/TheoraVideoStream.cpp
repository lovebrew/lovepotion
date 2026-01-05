#include "common/math.hpp"
#include "common/pixelformat.hpp"

#include "modules/video/theora/TheoraVideoStream.hpp"

#include <array>
#include <iostream>
#include <utility>

#if defined(__3DS__)
    #include <citro3d.h>
#endif

namespace love
{
    TheoraVideoStream::TheoraVideoStream(FileBase* file) :
        demuxer(file),
        headerParsed(false),
        decoder(nullptr),
        frameReady(false),
        lastFrame(0.0),
        nextFrame(0.0)
    {
        if (this->demuxer.findStream() != OggDemuxer::TYPE_THEORA)
            throw Exception("Invalid video file, video is not theora.");

        th_info_init(&this->videoInfo);

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
            th_info_clear(&this->videoInfo);
            throw e;
        }

        this->frameSync.set(new DeltaSync(), Acquire::NO_RETAIN);
    }

    TheoraVideoStream::~TheoraVideoStream()
    {
        if (this->decoder)
            th_decode_free(this->decoder);

        th_info_clear(&this->videoInfo);

#if defined(__3DS__)
        Y2RU_StopConversion();
#endif

        delete this->frontBuffer;
        delete this->backBuffer;
    }

    int TheoraVideoStream::getWidth() const
    {
        if (this->headerParsed)
            return this->videoInfo.pic_width;

        return 0;
    }

    int TheoraVideoStream::getHeight() const
    {
        if (this->headerParsed)
            return this->videoInfo.pic_height;

        return 0;
    }

    const std::string& TheoraVideoStream::getFilename() const
    {
        return this->demuxer.getFilename();
    }

    void TheoraVideoStream::setSync(FrameSync* sync)
    {
        std::unique_lock lock(this->bufferMutex);
        this->frameSync = sync;
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
        return this->frameSync->isPlaying() && !this->demuxer.isEos();
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

        th_comment comment;
        th_setup_info* setupInfo = nullptr;
        th_comment_init(&comment);

        int result;

        this->demuxer.readPacket(this->packet);
        result = th_decode_headerin(&this->videoInfo, &comment, &setupInfo, &this->packet);

        if (result < 0)
        {
            th_comment_clear(&comment);
            throw love::Exception("Could not find header.");
        }

        while (result > 0)
        {
            this->demuxer.readPacket(this->packet);
            result = th_decode_headerin(&this->videoInfo, &comment, &setupInfo, &this->packet);
        }
        th_comment_clear(&comment);

        this->decoder = th_decode_alloc(&this->videoInfo, setupInfo);
        th_setup_free(setupInfo);

        std::array<Frame*, 2> buffers = { backBuffer, frontBuffer };

#if !defined(__3DS__)
        yPlaneXOffset = cPlaneXOffset = this->videoInfo.pic_x;
        yPlaneYOffset = cPlaneYOffset = this->videoInfo.pic_y;
        scaleFormat(this->videoInfo.pixel_fmt, cPlaneXOffset, cPlaneYOffset);

        for (int index = 0; index < buffers.size(); index++)
        {
            buffers[index]->cw = buffers[index]->yw = this->videoInfo.pic_width;
            buffers[index]->ch = buffers[index]->yh = this->videoInfo.pic_height;
            scaleFormat(this->videoInfo.pixel_fmt, buffers[index]->cw, buffers[index]->ch);

            buffers[index]->yPlane  = new uint8_t[buffers[index]->yw * buffers[index]->yh];
            buffers[index]->cbPlane = new uint8_t[buffers[index]->cw * buffers[index]->ch];
            buffers[index]->crPlane = new uint8_t[buffers[index]->cw * buffers[index]->ch];

            // clang-format off
            std::memset(buffers[index]->yPlane,   16, buffers[index]->yw * buffers[index]->yh);
            std::memset(buffers[index]->cbPlane, 128, buffers[index]->cw * buffers[index]->ch);
            std::memset(buffers[index]->crPlane, 128, buffers[index]->cw * buffers[index]->ch);
            // clang-format on
        }
#else
        const auto size = getPixelFormatSliceSize(PIXELFORMAT_RGBA8_UNORM, this->videoInfo.pic_width,
                                                  this->videoInfo.pic_height);

        for (int index = 0; index < buffers.size(); index++)
        {
            buffers[index]->cw = buffers[index]->yw = this->videoInfo.pic_width;
            buffers[index]->ch = buffers[index]->yh = this->videoInfo.pic_height;

            buffers[index]->data = new uint8_t[size];
            std::memset(buffers[index]->data, 0, size);
        }
#endif

        this->headerParsed = true;
        th_decode_packetin(this->decoder, &this->packet, nullptr);
    }

    void TheoraVideoStream::seekDecoder(double target)
    {
        auto getTime = [this](int64_t position) { return th_granule_time(this->decoder, position); };
        bool success = this->demuxer.seek(this->packet, target, getTime);

        if (!success)
            return;

        this->lastFrame = this->nextFrame = -1;
        th_decode_ctl(this->decoder, TH_DECCTL_SET_GRANPOS, &packet.granulepos, sizeof(packet.granulepos));
    }

    void TheoraVideoStream::threadedFillBackBuffer(double dt)
    {
        this->frameSync->update(dt);
        double position = this->frameSync->getPosition();

        if (position < this->lastFrame)
            this->seekDecoder(position);

        th_ycbcr_buffer buffer;
        bool hasFrame = false;

        uint32_t framesBehind = 0;
        bool failedSeek       = false;

        while (!this->demuxer.isEos() && position >= this->nextFrame)
        {
            if (framesBehind++ > 5 && !failedSeek)
            {
                this->seekDecoder(position);
                framesBehind = 0;
                failedSeek   = true;
            }

            th_decode_ycbcr_out(this->decoder, buffer);
            hasFrame = true;

            ogg_int64_t decoderPosition;

            do
            {
                if (this->demuxer.readPacket(packet))
                    return;

                // clang-format off
                if (this->packet.granulepos > 0)
                    th_decode_ctl(this->decoder, TH_DECCTL_SET_GRANPOS, &packet.granulepos, sizeof(packet.granulepos));
                // clang-format on

            } while (th_decode_packetin(this->decoder, &this->packet, &decoderPosition) != 0);

            this->lastFrame = this->nextFrame;
            this->nextFrame = th_granule_time(this->decoder, decoderPosition);
        }

        if (hasFrame)
        {
            {
                std::unique_lock lock(this->bufferMutex);
                this->frameReady = false;
            }

#if !defined(__3DS__)
            // clang-format off
            for (int y = 0; y < this->backBuffer->yh; ++y)
            {
                const auto* src = buffer[0].data + buffer[0].stride * (y + this->yPlaneYOffset) + this->yPlaneXOffset;
                std::memcpy(this->backBuffer->yPlane + this->backBuffer->yw * y, src, this->backBuffer->yw);
            }

            for (int y = 0; y < this->backBuffer->ch; ++y)
            {
                const auto* src = buffer[1].data + buffer[1].stride * (y + this->cPlaneYOffset) + this->cPlaneXOffset;
                std::memcpy(this->backBuffer->cbPlane + this->backBuffer->cw * y, src, this->backBuffer->cw);
            }

            for (int y = 0; y < this->backBuffer->ch; ++y)
            {
                const auto* src = buffer[2].data + buffer[2].stride * (y + this->cPlaneYOffset) + this->cPlaneXOffset;
                std::memcpy(this->backBuffer->crPlane + this->backBuffer->cw * y, src, this->backBuffer->cw);
            }
            // clang-format on
#else
            bool isBusy = true;
            if (!buffer[0].data || !buffer[1].data || !buffer[2].data)
                return;

            Y2RU_StopConversion();

            while (isBusy)
                Y2RU_IsBusyConversion(&isBusy);

            switch (this->videoInfo.pixel_fmt)
            {
                case TH_PF_420:
                    Y2RU_SetInputFormat(INPUT_YUV420_INDIV_8);
                    break;
                case TH_PF_422:
                    Y2RU_SetInputFormat(INPUT_YUV422_INDIV_8);
                    break;
                default:
                    break;
            }

            // clang-format off
            const auto width  = ((this->videoInfo.pic_x + this->videoInfo.frame_width  + 1) & ~1) - (this->videoInfo.pic_x & ~1);
            const auto height = ((this->videoInfo.pic_y + this->videoInfo.frame_height + 1) & ~1) - (this->videoInfo.pic_y & ~1);
            // clang-format on

            Y2RU_SetOutputFormat(OUTPUT_RGB_32);
            Y2RU_SetRotation(ROTATION_NONE);
            Y2RU_SetBlockAlignment(BLOCK_8_BY_8);
            Y2RU_SetTransferEndInterrupt(true);
            Y2RU_SetInputLineWidth(width);
            Y2RU_SetInputLines(height);
            Y2RU_SetStandardCoefficient(COEFFICIENT_ITU_R_BT_601_SCALING);
            Y2RU_SetAlpha(0xFF);

            // clang-format off
            Y2RU_SetSendingY(buffer[0].data, width * height, width, buffer[0].stride - width);
            Y2RU_SetSendingU(buffer[1].data, (width / 2) * (height / 2), width / 2, buffer[1].stride - (width >> 1));
            Y2RU_SetSendingV(buffer[2].data, (width / 2) * (height / 2), width / 2, buffer[2].stride - (width >> 1));

            const auto formatSize = getPixelFormatBlockSize(PIXELFORMAT_RGBA8_UNORM);
            Y2RU_SetReceiving(this->backBuffer->data, width * height * formatSize, width * 8 * formatSize, (NextPo2(width) - width) * 8 * formatSize);
            // clang-format on

            Y2RU_StartConversion();

            Y2RU_GetTransferEndEvent(&this->handle);
            if (svcWaitSynchronization(this->handle, 6e7))
                std::printf("y2r:u timed out.\n");
#endif

            {
                std::unique_lock lock(this->bufferMutex);
                this->frameReady = true;
            }
        }
    }

    void TheoraVideoStream::fillBackBuffer()
    {}

    bool TheoraVideoStream::swapBuffers()
    {
        if (this->demuxer.isEos())
            return false;

        if (!this->frameSync->isPlaying())
            return false;

        std::unique_lock lock(this->bufferMutex);
        if (!this->frameReady)
            return false;

        this->frameReady = false;

        std::swap(this->frontBuffer, this->backBuffer);
        return true;
    }
} // namespace love
