#pragma once

#include <objects/texture/texture.tcc>

#include <utilities/driver/renderer_ext.hpp>

#include <gx2/texture.h>

namespace love
{
    template<>
    class Texture<Console::CAFE> : public Texture<Console::ALL>
    {
      public:
        Texture(const Graphics<Console::CAFE>* graphics, const Settings& settings,
                const Slices* data);

        virtual ~Texture();

        virtual void Draw(Graphics<Console::CAFE>& graphics,
                          const Matrix4<Console::Which>& matrix) override;

        virtual void Draw(Graphics<Console::CAFE>& graphics, Quad* quad,
                          const Matrix4<Console::Which>& transform) override;

        void ReplacePixels(ImageData<Console::CAFE>* data, int slice, int mipmap, int x, int y,
                           bool reloadMipmaps);

        void ReplacePixels(const void* data, size_t size, int slice, int mipmap, const Rect& rect,
                           bool reloadMipmaps);

        void SetSamplerState(const SamplerState& state);

        void GenerateMipmaps()
        {}

        bool LoadVolatile();

        void UnloadVolatile();

        GX2Sampler& GetSampler()
        {
            return this->sampler;
        }

        GX2Texture* GetHandle()
        {
            return this->texture;
        }

        GX2ColorBuffer* GetFramebuffer()
        {
            return this->framebuffer;
        }

      private:
        void CreateTexture();

        GX2ColorBuffer* framebuffer;
        GX2Texture* texture;

        GX2Sampler sampler;
    };
} // namespace love
