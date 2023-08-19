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
        static constexpr int MAX_OBJECTS        = 0x1000;
        static constexpr int VERTEX_BUFFER_SIZE = 6 * MAX_OBJECTS;

        DrawBuffer()
        {}

        DrawBuffer(size_t count) : DrawBuffer<Console::ALL>(count)
        {}

        static void Init()
        {
            BufInfo_Init(&info);

            vertices = (Vertex*)linearAlloc(VERTEX_BUFFER_SIZE * VERTEX_SIZE);

            if (!vertices)
                throw love::Exception("Out of memory.");

            int result = BufInfo_Add(&info, (void*)vertices, VERTEX_SIZE, 0x03, 0x210);

            if (result < 0)
                throw love::Exception("Failed to add C3D_BufInfo.");

            initialized = true;
        }

        ~DrawBuffer()
        {
            if (this->vertices)
                linearFree(this->vertices);
        }

        void SetBufferInfo()
        {
            C3D_SetBufInfo(&this->info);
        }

        C3D_BufInfo* GetBuffer()
        {
            return &this->info;
        }

      private:
        static inline C3D_BufInfo info {};
    };
} // namespace love