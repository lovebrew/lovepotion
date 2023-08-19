#pragma once

#include <common/color.hpp>

#include <utilities/driver/drawbuffer_ext.hpp>
#include <utilities/driver/renderer/drawcommand.tcc>

namespace love
{
    template<>
    struct DrawCommand<Console::CTR> : public DrawCommand<Console::ALL>
    {
      public:
        enum TEXENV_MODE
        {
            TEXENV_MODE_PRIMITIVE,
            TEXENV_MODE_TEXTURE,
            TEXENV_MODE_TEXT,
            TEXENV_MODE_MAX_ENUM
        };

        static inline int m_vertexOffset = 0;

        DrawCommand()
        {}

        DrawCommand(size_t count, PrimitiveType type = PRIMITIVE_TRIANGLES,
                    Shader<>::StandardShader shader = Shader<>::STANDARD_DEFAULT) :
            DrawCommand<Console::ALL>(count, type, shader)
        {}

        void FillVertices(const Color& color)
        {
            this->SetTexEnv(TEXENV_MODE_PRIMITIVE);

            this->buffer  = std::make_shared<DrawBuffer<Console::CTR>>(this->count);
            auto vertices = this->buffer->GetVertices() + m_vertexOffset;

            DrawCommand<Console::ALL>::FillVertices(vertices, color);

            this->buffer->SetBufferInfo();
            m_vertexOffset += this->count;
        }

        void FillVertices(const Color* colors)
        {
            this->SetTexEnv(TEXENV_MODE_PRIMITIVE);

            this->buffer  = std::make_shared<DrawBuffer<Console::CTR>>(this->count);
            auto vertices = this->buffer->GetVertices() + m_vertexOffset;

            DrawCommand<Console::ALL>::FillVertices(vertices, colors);

            this->buffer->SetBufferInfo();
            m_vertexOffset += this->count;
        }

        void FillVertices(std::span<Color> colors)
        {
            this->SetTexEnv(TEXENV_MODE_PRIMITIVE);

            this->buffer  = std::make_shared<DrawBuffer<Console::CTR>>(this->count);
            auto vertices = this->buffer->GetVertices() + m_vertexOffset;

            DrawCommand<Console::ALL>::FillVertices(vertices, colors);

            this->buffer->SetBufferInfo();
            m_vertexOffset += this->count;
        }

        void FillVertices(const Color& color, const Vector2* textureCoords)
        {
            this->SetTexEnv(TEXENV_MODE_TEXTURE);

            this->buffer  = std::make_shared<DrawBuffer<Console::CTR>>(this->count);
            auto vertices = this->buffer->GetVertices() + m_vertexOffset;

            DrawCommand<Console::ALL>::FillVertices(vertices, color, textureCoords);

            this->buffer->SetBufferInfo();
            m_vertexOffset += this->count;
        }

        void FillVertices(const Vertex* source)
        {
            this->SetTexEnv(TEXENV_MODE_TEXT);

            this->buffer  = std::make_shared<DrawBuffer<Console::CTR>>(this->count);
            auto vertices = this->buffer->GetVertices() + m_vertexOffset;

            DrawCommand<Console::ALL>::FillVertices(vertices, source);

            this->buffer->SetBufferInfo();
            m_vertexOffset += this->count;
        }

      private:
        static inline TEXENV_MODE m_texEnvMode = TEXENV_MODE_MAX_ENUM;

        void SetTexEnv(TEXENV_MODE mode)
        {
            if (m_texEnvMode == mode)
                return;

            C3D_TexEnv* env = C3D_GetTexEnv(0);
            C3D_TexEnvInit(env);

            switch (mode)
            {
                case TEXENV_MODE_PRIMITIVE:
                {
                    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR,
                                  GPU_PRIMARY_COLOR);
                    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
                    break;
                }
                case TEXENV_MODE_TEXTURE:
                {
                    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR,
                                  GPU_PRIMARY_COLOR);
                    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
                    break;
                }
                case TEXENV_MODE_TEXT:
                {
                    C3D_TexEnvSrc(env, C3D_RGB, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR,
                                  GPU_PRIMARY_COLOR);
                    C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);

                    C3D_TexEnvSrc(env, C3D_Alpha, GPU_PRIMARY_COLOR, GPU_TEXTURE0,
                                  GPU_PRIMARY_COLOR);
                    C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);

                    break;
                }
                default:
                    throw love::Exception("Not allowed.");
            }

            m_texEnvMode = mode;
        }
    };
} // namespace love