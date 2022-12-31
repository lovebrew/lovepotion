#pragma once

#include <modules/graphics/graphics.tcc>

#include <objects/font_ext.hpp>
#include <objects/rasterizer_ext.hpp>
#include <objects/shader_ext.hpp>

#include <utilities/driver/vertex_ext.hpp>

#include <gx2/mem.h>
#include <gx2r/buffer.h>

namespace love
{
    template<>
    class Graphics<Console::CAFE> : public Graphics<Console::ALL>
    {
      public:
        struct DrawCommand
        {
          public:
            DrawCommand(int vertexCount) :
                buffer {},
                handles {},
                format(vertex::CommonFormat::PRIMITIVE),
                primitveType(vertex::PRIMITIVE_TRIANGLES),
                shader(Shader<>::STANDARD_DEFAULT)
            {
                try
                {
                    this->positions = std::make_unique<Vector2[]>(vertexCount);
                }
                catch (std::bad_alloc&)
                {
                    throw love::Exception("Out of memory.");
                }

                this->buffer.elemCount = vertexCount;
                this->buffer.elemSize  = vertex::VERTEX_SIZE;
                this->buffer.flags     = DrawCommand::GX2R_BUFFER_FLAGS;

                GX2RCreateBuffer(&this->buffer);
            }

            ~DrawCommand()
            {
                GX2RDestroyBufferEx(&this->buffer, GX2R_RESOURCE_BIND_NONE);
            }

            const std::unique_ptr<Vector2[]>& Positions() const
            {
                return this->positions;
            }

            GX2RBuffer& GetBuffer()
            {
                return this->buffer;
            }

            void FillVertices(const Color& color, const Vector2* textureCoords)
            {
                auto* vertices =
                    (vertex::Vertex*)GX2RLockBufferEx(&this->buffer, GX2R_RESOURCE_BIND_NONE);

                for (size_t index = 0; index < this->buffer.elemCount; index++)
                {
                    // clang-format off
                    vertices[index] =
                    {
                        .position = { this->positions[index].x, this->positions[index].y, 0 },
                        .color    = { color.r, color.g, color.b, color.a },
                        .texcoord = { textureCoords[index].x, textureCoords[index].y }
                    };
                    // clang-format on
                }

                GX2RUnlockBufferEx(&this->buffer, GX2R_RESOURCE_BIND_NONE);
            }

            GX2RBuffer buffer;

            std::unique_ptr<Vector2[]> positions;
            std::vector<Texture<Console::CAFE>*> handles;

            vertex::CommonFormat format;
            vertex::PrimitiveType primitveType;
            Shader<>::StandardShader shader;

          private:
            static constexpr auto GX2R_BUFFER_FLAGS =
                GX2R_RESOURCE_BIND_VERTEX_BUFFER | GX2R_RESOURCE_USAGE_CPU_READ |
                GX2R_RESOURCE_USAGE_CPU_WRITE | GX2R_RESOURCE_USAGE_GPU_READ;
        };

        static constexpr const char* DEFAULT_SCREEN = "tv";

        Graphics();

        void Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth);

        void Clear(std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth);

        void Present();

        bool SetMode(int x, int y, int width, int height);

        void CheckSetDefaultFont();

        void RestoreStateChecked(const DisplayState& state);

        void Pop();

        void Reset();

        void RestoreState(const DisplayState& state);

        Font<Console::CAFE>* NewFont(Rasterizer<Console::CAFE>* data) const;

        Font<Console::CAFE>* NewDefaultFont(int size,
                                            Rasterizer<Console::CAFE>::Hinting hinting) const;

        Texture<Console::CAFE>* NewTexture(const Texture<>::Settings& settings,
                                           const Texture<>::Slices* slices = nullptr) const;

        void Draw(Texture<Console::CAFE>* texture, Quad* quad,
                  const Matrix4<Console::CAFE>& matrix);

        void Draw(Drawable* drawable, const Matrix4<Console::CAFE>& matrix);

        void Print(const Font<>::ColoredStrings& strings, const Matrix4<Console::CAFE>& matrix);

        void Print(const Font<>::ColoredStrings& strings, Font<Console::CAFE>* font,
                   const Matrix4<Console::CAFE>& matrix);

        void Printf(const Font<>::ColoredStrings& strings, float wrap, Font<>::AlignMode align,
                    const Matrix4<Console::CAFE>& matrix);

        void Printf(const Font<>::ColoredStrings& strings, Font<Console::CAFE>* font, float wrap,
                    Font<>::AlignMode align, const Matrix4<Console::CAFE>& matrix);

        void SetScissor();

        void SetScissor(const Rect& scissor);

        int GetWidth(Screen screen) const;

        int GetHeight(Screen screen) const;

        void SetViewportSize(int width, int height);

        void SetShader()
        {
            Shader<Console::CAFE>::AttachDefault(Shader<>::STANDARD_DEFAULT);
            this->states.back().shader.Set(nullptr);
        }

        void SetShader(Shader<Console::CAFE>* shader)
        {
            if (shader == nullptr)
                return this->SetShader();

            shader->Attach();
            this->states.back().shader.Set(shader);
        }
    }; // namespace love
} // namespace love
