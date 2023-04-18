#include <objects/texture/texture.tcc>

#include <modules/graphics_ext.hpp>

#include <citro2d.h>

namespace love
{
    template<>
    class Texture<Console::CTR> : public Texture<Console::ALL>
    {
      public:
        Texture(const Graphics<Console::ALL>* graphics, const Settings& settings,
                const Slices* data);

        virtual ~Texture();

        virtual void Draw(Graphics<Console::CTR>& graphics,
                          const Matrix4<Console::Which>& matrix) override;

        virtual void Draw(Graphics<Console::CTR>& graphics, Quad* quad,
                          const Matrix4<Console::Which>& transform) override;

        void ReplacePixels(ImageData<Console::CTR>* data, int slice, int mipmap, int x, int y,
                           bool reloadMipmaps);

        void ReplacePixels(const void* data, size_t size, int slice, int mipmap, const Rect& rect,
                           bool reloadMipmaps);

        void SetSamplerState(const SamplerState& state);

        void GenerateMipmaps()
        {}

        bool LoadVolatile();

        void UnloadVolatile();

        C3D_Tex* GetHandle()
        {
            return this->image.tex;
        }

        C3D_RenderTarget* GetRenderTargetHandle() const
        {
            if (this->renderTarget)
                return this->framebuffer;

            /* shouldn't happen */
            return nullptr;
        }

      private:
        void CreateTexture();

        C2D_Image image;
        C3D_RenderTarget* framebuffer;
    };
} // namespace love
