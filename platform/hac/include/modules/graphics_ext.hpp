#pragma once

#include <modules/graphics/graphics.tcc>

#include <objects/font_ext.hpp>
#include <objects/rasterizer_ext.hpp>
#include <objects/shader_ext.hpp>

#include <utilities/driver/vertex_ext.hpp>

namespace love
{
    template<>
    class Graphics<Console::HAC> : public Graphics<Console::ALL>
    {
      public:
        static constexpr const char* DEFAULT_SCREEN = "default";

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
                LOG("%zu", this->count);
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

            std::vector<DkResHandle> handles;

            vertex::CommonFormat format;
            vertex::PrimitiveType primitveType;
            Shader<>::StandardShader shader;
        };

        Graphics();

        virtual ~Graphics();

        void Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth);

        void Clear(std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth);

        void Present();

        bool SetMode(int x, int y, int width, int height);

        void CheckSetDefaultFont();

        void RestoreStateChecked(const DisplayState& state);

        void Pop();

        void SetShader()
        {
            Shader<Console::HAC>::AttachDefault(Shader<>::STANDARD_DEFAULT);
            this->states.back().shader.Set(nullptr);
        }

        void SetShader(Shader<Console::HAC>* shader)
        {
            if (shader == nullptr)
                return this->SetShader();

            shader->Attach();
            this->states.back().shader.Set(shader);
        }

        void Reset();

        void RestoreState(const DisplayState& state);

        Font<Console::HAC>* NewFont(Rasterizer<Console::HAC>* data) const;

        Font<Console::HAC>* NewDefaultFont(int size,
                                           Rasterizer<Console::HAC>::Hinting hinting) const;

        Texture<Console::HAC>* NewTexture(const Texture<>::Settings& settings,
                                          const Texture<>::Slices* slices = nullptr) const;

        void Draw(Texture<Console::HAC>* texture, Quad* quad, const Matrix4<Console::HAC>& matrix);

        void Draw(Drawable* drawable, const Matrix4<Console::HAC>& matrix);

        void Print(const Font<>::ColoredStrings& strings, const Matrix4<Console::HAC>& matrix);

        void Print(const Font<>::ColoredStrings& strings, Font<Console::HAC>* font,
                   const Matrix4<Console::HAC>& matrix);

        void Printf(const Font<>::ColoredStrings& strings, float wrap, Font<>::AlignMode align,
                    const Matrix4<Console::HAC>& matrix);

        void Printf(const Font<>::ColoredStrings& strings, Font<Console::HAC>* font, float wrap,
                    Font<>::AlignMode align, const Matrix4<Console::HAC>& matrix);

        void SetScissor();

        void SetScissor(const Rect& scissor);

        int GetWidth(Screen screen) const
        {
            return this->width;
        }

        int GetHeight(Screen screen) const
        {
            return this->height;
        }

        void SetViewportSize(int width, int height);
    }; // namespace love
} // namespace love
