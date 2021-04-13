#pragma once

#include "objects/canvas/canvasc.h"

namespace love
{
    class Canvas : public common::Canvas
    {
      public:
        Canvas(const Settings& settings);

        virtual ~Canvas();

        void Draw(Graphics* gfx, Quad* quad, const Matrix4& localTransform) override;

        constexpr dk::Image& GetImage()
        {
            return this->colorBuffer;
        }

        constexpr dk::ImageDescriptor const& GetDescriptor() const
        {
            return this->descriptor;
        }

      private:
        dk::Image colorBuffer;
        CMemPool::Handle colorMemory;

        dk::ImageDescriptor descriptor;
    };
} // namespace love