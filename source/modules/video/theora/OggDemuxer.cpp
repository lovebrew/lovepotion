#include "modules/video/theora/OggDemuxer.hpp"
#include "common/Exception.hpp"
#include "modules/filesystem/File.tcc"
#include <cstdint>
#include <cstring>
#include <ogg/ogg.h>

namespace love
{
    OggDemuxer::OggDemuxer(FileBase* file) : file(file), streamInited(false), videoSerial(0), eos(false)
    {
        ogg_sync_init(&this->sync);
    }

    OggDemuxer::~OggDemuxer()
    {
        if (this->streamInited)
            ogg_stream_clear(&this->stream);
        ogg_sync_clear(&this->sync);
    }

    bool OggDemuxer::readPage(bool erroreof)
    {
        char* buffer = nullptr;
        while (ogg_sync_pageout(&this->sync, &this->page) != 1)
        {
            if (buffer && !this->streamInited && ogg_stream_check(&this->stream))
                throw love::Exception("Invalid stream.");

            buffer      = ogg_sync_buffer(&this->sync, 8192);
            size_t read = this->file->read(buffer, 8192);
            if (read == 0 && erroreof)
                return false;

            ogg_sync_wrote(&this->sync, read);
        }

        return true;
    }

    bool OggDemuxer::readPacket(ogg_packet& packet, bool mustSucceed)
    {
        if (!this->streamInited)
            throw love::Exception("Reading from OggDemuxer before initialized!");

        while (ogg_stream_packetout(&this->stream, &packet) != 1)
        {
            do
            {
                if (ogg_page_serialno(&page) == this->videoSerial && ogg_page_eos(&this->page) &&
                    !mustSucceed)
                    return this->eos = true;

                this->readPage();
            } while (ogg_page_serialno(&this->page) != this->videoSerial);

            ogg_stream_pagein(&this->stream, &this->page);
        }

        return this->eos = false;
    }

    void OggDemuxer::resync()
    {
        ogg_sync_reset(&this->sync);
        ogg_sync_pageseek(&this->sync, &this->page);
        ogg_stream_reset(&this->stream);
    }

    bool OggDemuxer::isEos() const
    {
        return this->eos;
    }

    const std::string& OggDemuxer::getFilename() const
    {
        return this->file->getFilename();
    }

    OggDemuxer::StreamType OggDemuxer::determineType()
    {
        ogg_packet packet;
        if (ogg_stream_packetpeek(&this->stream, &packet) != 1)
            return TYPE_UNKNOWN;

        if (packet.bytes >= 7)
        {
            uint8_t header = packet.packet[0];
            if (header & 0x80 && std::strncmp((const char*)packet.packet + 1, "theora", 6) == 0)
                return TYPE_THEORA;
        }

        return TYPE_UNKNOWN;
    }

    OggDemuxer::StreamType OggDemuxer::findStream()
    {
        if (this->streamInited)
        {
            this->eos          = false;
            this->streamInited = false;
            this->file->seek(0);
            ogg_stream_clear(&this->stream);
            ogg_sync_reset(&this->sync);
        }

        while (true)
        {
            if (!this->readPage(true))
                return TYPE_UNKNOWN;

            if (!ogg_page_bos(&this->page))
                break;

            this->videoSerial = ogg_page_serialno(&this->page);
            ogg_stream_init(&this->stream, this->videoSerial);
            ogg_stream_pagein(&this->stream, &this->page);
            this->streamInited = true;

            auto type = this->determineType();
            switch (type)
            {
                case TYPE_THEORA:
                    return type;
                default:
                    break;
            }
            ogg_stream_clear(&this->stream);
            this->streamInited = false;
        }

        if (this->streamInited)
        {
            this->streamInited = false;
            ogg_stream_clear(&this->stream);
        }

        ogg_sync_reset(&this->sync);
        return TYPE_UNKNOWN;
    }

    bool OggDemuxer::seek(ogg_packet& packet, double target, std::function<double(int64_t)> getTime)
    {
        static const double threshold = 0.01;
        this->eos                     = false;

        if (target < threshold)
        {
            this->file->seek(0);
            this->resync();
            this->readPacket(packet, true);
            return true;
        }

        double low  = 0;
        double high = this->file->getSize();

        if (packet.granulepos != -1)
        {
            double current = getTime(packet.granulepos);
            if (current < target)
                low = this->file->tell();
            else if (current > target)
                high = this->file->tell();
        }

        while (high - low > threshold)
        {
            double position = (high + low) / 2;
            this->file->seek(position);

            this->resync();

            this->readPage();
            this->readPacket(packet, false);

            if (this->isEos())
            {
                high      = position;
                this->eos = false;

                if (high < threshold)
                {
                    this->file->seek(0);
                    this->resync();
                    this->readPacket(packet, true);
                }
                else
                    continue;
            }

            int result = -1;
            for (int i = 0; i < ogg_page_packets(&this->page); ++i)
            {
                if (i > 0)
                    this->readPacket(packet, true);

                double current = getTime(packet.granulepos);
                double next    = getTime(packet.granulepos + 1);

                if (current == -1)
                    continue;
                else if (current <= target && next > target)
                {
                    result = 0;
                    break;
                }
                else if (current > target)
                {
                    result = 1;
                    break;
                }
            }

            if (result == 0)
                break;
            else if (result < 0)
                low = position;
            else
                high = position;
        }

        return true;
    }
} // namespace love
