#include "modules/video/theora/OggDemuxer.hpp"

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

    bool OggDemuxer::readPage(bool errorEof)
    {
        char* syncBuffer = nullptr;
        while (ogg_sync_pageout(&this->sync, &this->page) != 1)
        {
            if (syncBuffer && !this->streamInited && ogg_stream_check(&this->stream))
                throw love::Exception("Invalid stream.");

            syncBuffer  = ogg_sync_buffer(&this->sync, 8192);
            size_t read = this->file->read(syncBuffer, 8192);
            if (read == 0 && errorEof)
                return false;

            ogg_sync_wrote(&this->sync, read);
        }

        return true;
    }

    bool OggDemuxer::readPacket(ogg_packet& packet, bool mustSucceed)
    {
        if (!this->streamInited)
            throw love::Exception("Reading from OggDemuxer before initialization (engine bug).");

        while (ogg_stream_packetout(&this->stream, &packet) != 1)
        {
            do
            {
                const auto serialno = ogg_page_serialno(&this->page);
                if (serialno == this->videoSerial && ogg_page_eos(&this->page) && !mustSucceed)
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
            const auto type = packet.packet[0];
            if (type == 0x80 && std::memcmp((const char*)packet.packet + 1, "theora", 6) == 0)
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
        this->eos = false;

        if (target < REWIND_THRESHOLD)
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
            double time = getTime(packet.granulepos);
            if (time < target)
                low = this->file->tell();
            else
                high = this->file->tell();
        }

        while (high - low > REWIND_THRESHOLD)
        {
            double position = (high + low) / 2.0;
            this->file->seek(position);

            this->resync();

            this->readPage();
            this->readPacket(packet, false);

            if (this->isEos())
            {
                high      = position;
                this->eos = false;

                if (high < REWIND_THRESHOLD)
                {
                    this->file->seek(0);
                    this->resync();
                    this->readPacket(packet, true);
                }
                else
                    continue;
            }

            int result = -1;
            for (int index = 0; index < ogg_page_packets(&this->page); ++index)
            {
                if (index > 0)
                    this->readPacket(packet, true);

                double time     = getTime(packet.granulepos);
                double nextTime = getTime(packet.granulepos + 1);

                if (time == -1)
                    continue;
                else if (time <= target && nextTime > target)
                {
                    result = 0;
                    break;
                }
                else if (time > target)
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
