#include "objects/videostream/theora/oggdemuxer.h"

using namespace love;

OggDemuxer::OggDemuxer(File* file) : file(file), intiialized(false), serial(0), endOfStream(false)
{
    ogg_sync_init(&this->sync);
}

OggDemuxer::~OggDemuxer()
{
    if (this->intiialized)
        ogg_stream_clear(&this->stream);

    ogg_sync_clear(&this->sync);
}

bool OggDemuxer::ReadPage(bool errorEOF)
{
    char* syncBuffer = nullptr;
    while (ogg_sync_pageout(&this->sync, &this->page) != 1)
    {
        if (syncBuffer && !this->intiialized && ogg_stream_check(&this->stream))
            throw love::Exception("Invalid stream.");

        syncBuffer  = ogg_sync_buffer(&sync, OggDemuxer::SYNC_VALUE);
        size_t read = this->file->Read(syncBuffer, OggDemuxer::SYNC_VALUE);

        if (read == 0 && errorEOF)
            return false;

        ogg_sync_wrote(&sync, read);
    }

    return true;
}

bool OggDemuxer::ReadPacket(ogg_packet& packet, bool mustSucceed)
{
    if (!this->intiialized)
        throw love::Exception("Reading from OggDemuxer before initialization! (engine bug)");

    while (ogg_stream_packetout(&this->stream, &packet) != 1)
    {
        do
        {
            if (ogg_page_serialno(&this->page) == this->serial && ogg_page_eos(&this->page) &&
                !mustSucceed)
            {
                return this->endOfStream = true;
            }

            this->ReadPage();
        } while (ogg_page_serialno(&this->page) != this->serial);

        ogg_stream_pagein(&this->stream, &page);
    }

    return this->endOfStream = false;
}

void OggDemuxer::ReSync()
{
    ogg_sync_reset(&this->sync);
    ogg_sync_pageseek(&this->sync, &this->page);
    ogg_stream_reset(&this->stream);
}

bool OggDemuxer::IsEOS() const
{
    return this->endOfStream;
}

const std::string& OggDemuxer::GetFilename() const
{
    return this->file->GetFilename();
}

OggDemuxer::StreamType OggDemuxer::DetermineType()
{
    ogg_packet packet;
    if (ogg_stream_packetpeek(&this->stream, &packet) != 1)
        return OggDemuxer::TYPE_UNKNOWN;

    if (packet.bytes >= OggDemuxer::THEORA_BYTES_MIN)
    {
        uint8_t headerType = packet.packet[0];
        if ((headerType & OggDemuxer::THEORA_HEADER_TYPE) &&
            (strncmp((const char*)packet.packet + 1, "theora", 6) == 0))
        {
            return OggDemuxer::TYPE_THEORA;
        }
    }

    return OggDemuxer::TYPE_UNKNOWN;
}

OggDemuxer::StreamType OggDemuxer::FindStream()
{
    if (this->intiialized)
    {
        this->endOfStream = false;
        this->intiialized = false;

        this->file->Seek(0);

        ogg_stream_clear(&this->stream);
        ogg_sync_reset(&this->sync);
    }

    while (true)
    {
        if (!this->ReadPage(true))
            return OggDemuxer::TYPE_UNKNOWN;

        if (!ogg_page_bos(&this->page))
            break;

        this->serial = ogg_page_serialno(&this->page);
        ogg_stream_init(&this->stream, this->serial);
        ogg_stream_pagein(&this->stream, &this->page);

        this->intiialized = true;

        StreamType type = this->DetermineType();

        switch (type)
        {
            case TYPE_THEORA:
                return type;
            default:
                break;
        }

        ogg_stream_clear(&this->stream);
        this->intiialized = false;
    }

    if (this->intiialized)
    {
        this->intiialized = false;
        ogg_stream_clear(&this->stream);
    }

    ogg_sync_reset(&this->sync);

    return OggDemuxer::TYPE_UNKNOWN;
}

bool OggDemuxer::Seek(ogg_packet& packet, double target, std::function<double(int64_t)> getTime)
{
    this->endOfStream = false;

    if (target < OggDemuxer::REWIND_THRESHOLD)
    {
        this->file->Seek(0);
        this->ReSync();
        this->ReadPacket(packet, true);

        return true;
    }

    double low  = 0;
    double high = this->file->GetSize();

    /*
    ** if we know our current position
    ** we can drastically decrease the search area
    */
    if (packet.granulepos != -1)
    {
        double currentTime = getTime(packet.granulepos);

        if (currentTime < target)
            low = file->Tell();
        else if (currentTime > target)
            high = file->Tell();
    }

    while (high - low > OggDemuxer::REWIND_THRESHOLD)
    {
        /* determine our next binary search position */
        double position = (high + low) / 2;
        this->file->Seek(position);

        /* do a sync */
        this->ReSync();

        this->ReadPage();
        this->ReadPacket(packet, false);

        if (this->IsEOS())
        {
            high              = position;
            this->endOfStream = false;

            /* fix for single-paged files */
            if (high < OggDemuxer::REWIND_THRESHOLD)
            {
                this->file->Seek(0);
                this->ReSync();
                this->ReadPacket(packet, true);
            }
            else
                continue;
        }

        int result = -1;
        for (int index = 0; index < ogg_page_packets(&this->page); ++index)
        {
            if (index > 0)
                this->ReadPacket(packet, true);

            double currentTime = getTime(packet.granulepos);
            double nextTime    = getTime(packet.granulepos + 1);

            if (currentTime == -1)
                continue;
            else if (currentTime <= target && nextTime > target)
            {
                result = 0;
                break;
            }
            else if (currentTime > target)
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
