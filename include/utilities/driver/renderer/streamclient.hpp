#pragma once

#include <common/exception.hpp>
#include <common/volatile.hpp>

#include <utilities/debug/logfile.hpp>
#include <utilities/driver/renderer/streambuffer.hpp>

#include <vector>

static constexpr int BUFFER_FRAMES = 0x04;

namespace love
{
    class StreamBufferClientMemory final : public StreamBuffer
    {
      public:
        StreamBufferClientMemory(vertex::BufferUsage mode, size_t size) :
            StreamBuffer(mode, size),
            data(nullptr)
        {
            try
            {
                this->data = new uint8_t[size];
            }
            catch (std::exception& e)
            {
                throw love::Exception("Out of memory.");
            }
        }

        virtual ~StreamBufferClientMemory()
        {
            delete[] this->data;
        }

        virtual MapInfo Map(size_t minsize) override
        {
            return MapInfo(this->data, this->bufferSize);
        }

        virtual size_t Unmap(size_t usedsize) override
        {
            return (size_t)this->data;
        }

        void MarkUsed(size_t /*usedsize*/) override
        {}

        ptrdiff_t GetHandle() const override
        {
            return 0;
        }

      private:
        uint8_t* data;
    };

#if defined(__3DS__)
    #include <3ds.h>

    class StreamBufferSubDataOrphan final : public StreamBuffer, public Volatile
    {
        StreamBufferSubDataOrphan(vertex::BufferUsage mode, size_t size) :
            StreamBuffer(mode, size),
            data(nullptr),
            orphan(false)
        {
            try
            {
                this->data = (uint8_t*)linearAlloc(size);
            }
            catch (std::exception& e)
            {
                throw love::Exception("Out of memory.");
            }

            this->LoadVolatile();
        }

        virtual ~StreamBufferSubDataOrphan()
        {
            this->UnloadVolatile();
            linearFree(this->data);
        }

        /* Maps some data to GPU memory */
        virtual MapInfo Map(size_t minSize) override
        {
            if (this->orphan)
            {
                this->orphan             = false;
                this->frameGPUReadOffset = 0;
                // glBindBuffer(type, buffer)?
                // glBufferData(mode, size, nullptr, usage)?
                // BufInfo_Add(this->vbo, this->data, VERTEX_SIZE, );
            }

            return MapInfo(this->data, this->bufferSize - this->frameGPUReadOffset);
        }

        /* Uploads data to the GPU */
        virtual size_t Unmap(size_t usedSize) override
        {
            BufInfo_Add(&this->vbo, this->data, VERTEX_SIZE, 0x03, 0x210);
            C3D_SetBufInfo(&this->vbo);

            return this->frameGPUReadOffset;
        }

        void MarkUsed(size_t usedSize) override
        {
            this->frameGPUReadOffset += usedSize;
        }

        void NextFrame() override
        {
            this->frameGPUReadOffset = 0;
            this->orphan             = true;
        }

        ptrdiff_t GetHandle() const override
        {
            return (ptrdiff_t)(&this->vbo);
        }

        bool LoadVolatile() override
        {
            if (this->loaded)
                return true;

            BufInfo_Init(&this->vbo);

            this->frameGPUReadOffset = 0;
            this->orphan             = false;
            this->loaded             = true;

            return true;
        }

        void UnloadVolatile() override
        {
            if (!this->loaded)
                return;

            BufInfo_Init(&this->vbo);
        }

      protected:
        C3D_BufInfo vbo;
        uint8_t* data;
        bool orphan;
        bool loaded;
    };
#endif

    static inline StreamBuffer* CreateStreamBuffer(BufferUsage mode, size_t size)
    {
        return new StreamBufferClientMemory(mode, size);
    }
} // namespace love
