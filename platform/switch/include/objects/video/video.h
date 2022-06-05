#pragma once

#include "common/render/vertex.h"
#include "objects/video/videoc.h"

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
        Vertex::PrimitiveVertex vertices[4];
    };
} // namespace love
