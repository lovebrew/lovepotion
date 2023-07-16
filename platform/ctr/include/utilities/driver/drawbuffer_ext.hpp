#pragma once

#include <utilities/driver/renderer/drawbuffer.tcc>

#include <citro3d.h>
#include <utilities/log/logfile.hpp>

namespace love
{
    template<>
    struct DrawBuffer<Console::CTR> : public DrawBuffer<Console::ALL>
    {
      public:
        DrawBuffer()
        {}

        DrawBuffer(size_t count) : DrawBuffer<Console::ALL>(count), info {}
        {
            BufInfo_Init(&this->info);
            this->vertices = (Vertex*)linearAlloc(this->size);

            int result = BufInfo_Add(&this->info, (void*)this->vertices, VERTEX_SIZE, 0x03, 0x210);

            if (result < 0)
                this->valid = false;
        }

        ~DrawBuffer()
        {
            if (this->vertices)
                linearFree(this->vertices);
        }

        void FlushGPUDataCache()
        {
            if (this->valid)
            {
                Result result = GSPGPU_FlushDataCache((void*)this->vertices, this->size);

                if (R_FAILED(result))
                    this->valid = false;
            }
        }

        C3D_BufInfo* GetBuffer()
        {
            return &this->info;
        }

      private:
        C3D_BufInfo info;
    };
} // namespace love