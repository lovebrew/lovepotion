#pragma once

#include "objects/canvas/canvasc.h"

namespace love
{
    class Canvas : public common::Canvas
    {
        public:
            Canvas(const Settings & settings);

            void SetAsTarget() override;

            void Clear(const Colorf & color) override;

            void Draw(Graphics * gfx, Quad * quad, const Matrix4 & localTransform) override;

            constexpr dk::Image & get()
            {
                return this->colorBuffer;
            }

            constexpr dk::ImageDescriptor const & getDescriptor() const
            {
                return this->descriptor;
            }

        private:
            dk::Image colorBuffer;
            CMemPool::Handle colorMemory;

            dk::ImageDescriptor descriptor
    };
}