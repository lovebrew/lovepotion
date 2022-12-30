#pragma once

#include <modules/graphics/graphics.tcc>

#include <objects/font_ext.hpp>
#include <objects/rasterizer_ext.hpp>
#include <objects/shader_ext.hpp>

#include <utilities/driver/vertex_ext.hpp>

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
                positions {},
                vertices {},
                count(vertexCount),
                size(vertexCount * vertex::VERTEX_SIZE),
                handles {},
                format(vertex::CommonFormat::PRIMITIVE),
                primitveType(vertex::PRIMITIVE_TRIANGLES),
                shader(Shader<>::STANDARD_DEFAULT)
            {
                try
                {
                    this->positions = std::make_unique<Vector2[]>(vertexCount);
                    this->vertices  = std::make_unique<vertex::Vertex[]>(vertexCount);
                }
                catch (std::bad_alloc&)
                {
                    throw love::Exception("Out of memory.");
                }

                this->stride = 28;
                if (!handles.empty())
                    this->stride = 36;
            }

            const std::unique_ptr<Vector2[]>& Positions() const
            {
                return this->positions;
            }

            const std::unique_ptr<vertex::Vertex[]>& Vertices() const
            {
                return this->vertices;
            }

            void FillVertices(const Color& color)
            {
                for (size_t index = 0; index < this->count; index++)
                {
                    // clang-format off
                    this->vertices[index] =
                    {
                        .position = { this->positions[index].x, this->positions[index].y, 0 },
                        .color    = color.array()
                    };
                    // clang-format on
                }
            }

            void FillVertices(const vertex::Vertex* data)
            {
                for (size_t index = 0; index < this->count; index++)
                {
                    // clang-format off
                    this->vertices[index] =
                    {
                        .position = { this->positions[index].x, this->positions[index].y, 0 },
                        .color    = data[index].color,
                        .texcoord = data[index].texcoord
                    };
                    // clang-format on
                }
            }

            void FillVertices(const Color& color, const Vector2* textureCoords)
            {
                for (size_t index = 0; index < this->count; index++)
                {
                    // clang-format off
                    this->vertices[index] =
                    {
                        .position = { this->positions[index].x, this->positions[index].y, 0 },
                        .color    = color.array(),
                        .texcoord = vertex::Normalize(textureCoords[index])
                    };
                    // clang-format on
                }
            }

            std::unique_ptr<Vector2[]> positions;
            std::unique_ptr<vertex::Vertex[]> vertices;

            size_t count;
            size_t size;
            size_t stride;

            std::vector<Texture<Console::CAFE>*> handles;

            vertex::CommonFormat format;
            vertex::PrimitiveType primitveType;
            Shader<>::StandardShader shader;
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
    };
} // namespace love
