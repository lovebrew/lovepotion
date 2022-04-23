#include "objects/videostream/theorastream.h"

#include "modules/thread/types/lock.h"

using namespace love;

TheoraStream::Frame::Frame() : yPlane(nullptr), cbPlane(nullptr), crPlane(nullptr)
{}

TheoraStream::Frame::~Frame()
{
    delete[] this->yPlane;
    delete[] this->cbPlane;
    delete[] this->crPlane;
}

TheoraStream::TheoraStream(File* file) : common::TheoraStream(file)
{
    this->frontBuffer = new Frame();
    this->backBuffer  = new Frame();

    th_info_init(&this->info);

    try
    {
        this->ParseHeader();
    }
    catch (love::Exception& exception)
    {
        delete this->backBuffer;
        delete this->frontBuffer;

        th_info_clear(&this->info);

        throw exception;
    }
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

size_t TheoraStream::GetSize() const
{
    return sizeof(Frame);
}

void TheoraStream::SetupBuffers()
{
    Frame* buffers[2] = { (Frame*)this->backBuffer, (Frame*)this->frontBuffer };

    this->yPlaneXOffset = this->cPlaneXOffset = this->info.pic_x;
    this->yPlaneYOffset = this->cPlaneYOffset = this->info.pic_y;

    scaleFormat(this->info.pixel_fmt, cPlaneXOffset, cPlaneYOffset);

    for (size_t index = 0; index < 2; index++)
    {
        buffers[index]->cw = buffers[index]->yw = this->info.pic_width;
        buffers[index]->ch = buffers[index]->yh = this->info.pic_height;

        scaleFormat(this->info.pixel_fmt, buffers[index]->cw, buffers[index]->ch);

        buffers[index]->yPlane  = new uint8_t[buffers[index]->yw * buffers[index]->yh];
        buffers[index]->cbPlane = new uint8_t[buffers[index]->cw * buffers[index]->ch];
        buffers[index]->crPlane = new uint8_t[buffers[index]->cw * buffers[index]->ch];

        memset(buffers[index]->yPlane, 16, buffers[index]->yw * buffers[index]->yh);
        memset(buffers[index]->cbPlane, 128, buffers[index]->cw * buffers[index]->ch);
        memset(buffers[index]->crPlane, 128, buffers[index]->cw * buffers[index]->ch);
    }
}

void TheoraStream::FillBufferData(th_ycbcr_buffer bufferInfo)
{
    if (!bufferInfo[0].data || !bufferInfo[1].data || !bufferInfo[2].data)
        return;

    Frame* frame = (Frame*)this->backBuffer;

    for (int y = 0; y < frame->yh; ++y)
    {
        memcpy(frame->yPlane + frame->yw * y,
               bufferInfo[0].data + bufferInfo[0].stride * (y + yPlaneYOffset) + yPlaneXOffset,
               frame->yw);
    }

    for (int y = 0; y < frame->ch; ++y)
    {
        memcpy(frame->cbPlane + frame->cw * y,
               bufferInfo[1].data + bufferInfo[1].stride * (y + cPlaneYOffset) + cPlaneXOffset,
               frame->cw);
    }

    for (int y = 0; y < frame->ch; ++y)
    {
        memcpy(frame->crPlane + frame->cw * y,
               bufferInfo[2].data + bufferInfo[2].stride * (y + cPlaneYOffset) + cPlaneXOffset,
               frame->cw);
    }
}
