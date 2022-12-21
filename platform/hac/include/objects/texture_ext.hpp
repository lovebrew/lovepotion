#pragma once

#include <objects/texture/texture.tcc>

#include <utilities/driver/renderer_ext.hpp>

namespace love
{
    template<>
    class Texture<Console::HAC> : public Texture<Console::ALL>
    {
      public:
        Texture(const Graphics<Console::HAC>* graphics, const Settings& settings,
                const Slices* data);

        virtual ~Texture();

        virtual void Draw(Graphics<Console::HAC>& graphics,
                          const Matrix4<Console::Which>& matrix) override;

        virtual void Draw(Graphics<Console::HAC>& graphics, Quad* quad,
                          const Matrix4<Console::Which>& transform) override;

        void ReplacePixels(ImageData<Console::HAC>* data, int slice, int mipmap, int x, int y,
                           bool reloadMipmaps);

        void ReplacePixels(const void* data, size_t size, int slice, int mipmap, const Rect& rect,
                           bool reloadMipmaps);

        void SetSamplerState(const SamplerState& state);

        void GenerateMipmaps()
        {}

        bool LoadVolatile();

        void UnloadVolatile();

        DkResHandle GetHandle() const
        {
            return this->textureHandle;
        }

        dk::Image GetImage()
        {
            return this->image;
        }

        dk::ImageDescriptor& GetDescriptor()
        {
            return this->descriptor;
        }

        dk::Sampler& GetSampler()
        {
            return this->sampler;
        }

      private:
        void CreateTexture();

        DkResHandle textureHandle;

        dk::Image image;

        dk::ImageDescriptor descriptor;
        dk::Sampler sampler;

        CMemPool::Handle handle;
    };
} // namespace love
