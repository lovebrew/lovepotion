#pragma once

#include "objects/video/videoc.h"

#include <citro2d.h>

namespace love
{
    class Video : public common::Video
    {
      public:
        Video(Graphics* graphics, VideoStream* stream, float dpiScale = 1.0f);

        virtual ~Video();

        void Draw(Graphics* graphics, const Matrix4& matrix) override;

      protected:
        void Update() override;

      private:
        C2D_Image current;
    };
} // namespace love
