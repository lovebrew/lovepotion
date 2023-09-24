#pragma once

#include <common/console.hpp>
#include <common/drawable.hpp>
#include <common/exception.hpp>
#include <common/math.hpp>
#include <common/object.hpp>
#include <common/strongreference.hpp>

#include <objects/compressedimagedata/compressedimagedata.hpp>
#include <objects/imagedata/imagedata.tcc>
#include <objects/quad/quad.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>

#include <algorithm>
#include <stddef.h>

namespace love
{
    using VectorMipmapLayers = std::vector<std::vector<StrongReference<ImageDataBase>>>;

    template<Console::Platform T = Console::ALL>
    class Texture : public Drawable
    {
      public:
        enum TextureType
        {
            TEXTURE_2D,
            TEXTURE_VOLUME,
            TEXTURE_2D_ARRAY,
            TEXTURE_CUBE
        };

        static inline Type type = Type("Texture", &Drawable::type);

        enum MipmapsMode
        {
            MIPMAPS_NONE,
            MIPMAPS_MANUAL,
            MIPMAPS_AUTO
        };

        enum SettingType
        {
            SETTING_WIDTH,
            SETTING_HEIGHT,
            SETTING_LAYERS,
            SETTING_MIPMAPS,
            SETTING_MIPMAP_COUNT,
            SETTING_FORMAT,
            SETTING_LINEAR,
            SETTING_TYPE,
            SETTING_DPI_SCALE,
            SETTING_MSAA,
            SETTING_RENDER_TARGET,
            SETTING_COMPUTE_WRITE,
            SETTING_READABLE,
            SETTING_MAX_ENUM
        };

        // Size and format will be overridden by ImageData when supplied.
        struct Settings
        {
            int width           = 1;
            int height          = 1;
            int layers          = 1; // depth for 3D textures
            TextureType type    = TEXTURE_2D;
            MipmapsMode mipmaps = MIPMAPS_NONE;
            PixelFormat format  = PIXELFORMAT_NORMAL;
            bool linear         = false;
            float dpiScale      = 1.0f;
            int msaa            = 1;
            bool renderTarget   = false;
            bool computeWrite   = false;
            int mipmapCount     = 0;
            std::optional<bool> readable;
        };

        struct Slices
        {
          public:
            Slices(TextureType type) : textureType(type)
            {}

            void Clear()
            {
                this->data.clear();
            }

            void Set(int slice, int mipmap, ImageDataBase* data)
            {
                if (this->textureType == TEXTURE_VOLUME)
                {
                    if (mipmap >= (int)this->data.size())
                        this->data.resize(mipmap + 1);

                    if (slice >= (int)this->data[mipmap].size())
                        this->data[mipmap].resize(slice + 1);

                    this->data[mipmap][slice].Set(data);
                }
                else
                {
                    if (slice >= (int)this->data.size())
                        this->data.resize(slice + 1);

                    if (mipmap >= (int)this->data[slice].size())
                        this->data[slice].resize(mipmap + 1);

                    this->data[slice][mipmap].Set(data);
                }
            }

            ImageDataBase* Get(int slice, int mipmap) const
            {
                if (slice < 0 || slice >= this->GetSliceCount(mipmap))
                    return nullptr;

                if (mipmap < 0 || mipmap >= this->GetMipmapCount(slice))
                    return nullptr;

                if (this->textureType == TEXTURE_VOLUME)
                    return this->data[mipmap][slice].Get();

                return this->data[slice][mipmap].Get();
            }

            TextureType GetTextureType() const
            {
                return this->textureType;
            }

            void Add(CompressedImageData* data, int startSlice, int startMipmap, bool addAllSlices,
                     bool addAllMipmaps)
            {
                int sliceCount  = addAllSlices ? data->GetSliceCount() : 1;
                int mipmapCount = addAllMipmaps ? data->GetMipmapCount() : 1;

                for (int mipmap = 0; mipmap < mipmapCount; mipmap++)
                {
                    for (int slice = 0; slice < sliceCount; slice++)
                    {
                        this->Set(startSlice + slice, startMipmap + mipmap,
                                  data->GetSlice(slice, mipmap));
                    }
                }
            }

            int GetSliceCount(int mipmap = 0) const
            {
                if (this->textureType == TEXTURE_VOLUME)
                {
                    if (mipmap < 0 || mipmap >= (int)this->data.size())
                        return 0;

                    return this->data[mipmap].size();
                }

                return this->data.size();
            }

            int GetMipmapCount(int slice = 0) const
            {
                if (this->textureType == TEXTURE_VOLUME)
                    return this->data.size();
                else
                {
                    if (slice < 0 || slice >= (int)data.size())
                        return 0;

                    return this->data[slice].size();
                }
            }

            bool Validate() const
            {
                int sliceCount  = this->GetSliceCount();
                int mipmapCount = this->GetMipmapCount(0);

                if (sliceCount == 0 || mipmapCount == 0)
                {
                    throw love::Exception(
                        "At least one ImageData or CompressedImageData is required.");
                }

                if (this->textureType == TEXTURE_CUBE && sliceCount != 6)
                    throw love::Exception("Cube textures must have exactly 6 sides.");

                auto* first = this->Get(0, 0);

                int width  = first->GetWidth();
                int height = first->GetHeight();

                int depth          = (this->textureType == TEXTURE_VOLUME) ? sliceCount : 1;
                PixelFormat format = first->GetFormat();

                int expectedMipmaps = Texture<>::GetTotalMipmapCount(width, height, depth);

                if (mipmapCount != expectedMipmaps && mipmapCount != 1)
                {
                    throw love::Exception(
                        "Texture does not have all required mipmap levels (expected %d, got %d)",
                        expectedMipmaps, mipmapCount);
                }

                if (this->textureType == TEXTURE_CUBE && width != height)
                {
                    throw love::Exception(
                        "Cube textures must have equal widths and heights for each cube face.");
                }

                int mipWidth  = width;
                int mipHeight = height;
                int mipSlices = sliceCount;

                for (int mipmap = 0; mipmap < mipmapCount; mipmap++)
                {
                    if (this->textureType == TEXTURE_VOLUME)
                    {
                        sliceCount = this->GetSliceCount(mipmap);

                        if (sliceCount != mipSlices)
                        {
                            throw love::Exception("Invalid number of image data layers in mipmap "
                                                  "level %d (expected %d, got %d)",
                                                  mipmap + 1, mipSlices, sliceCount);
                        }
                    }

                    for (int slice = 0; slice < sliceCount; slice++)
                    {
                        auto* sliceData = this->Get(slice, mipmap);

                        if (!sliceData)
                        {
                            throw love::Exception("Missing image data (slice %d, mipmap level %d)",
                                                  slice + 1, mipmap + 1);
                        }

                        int realWidth  = sliceData->GetWidth();
                        int realHeight = sliceData->GetHeight();

                        if (this->GetMipmapCount(slice) != mipmapCount)
                        {
                            throw love::Exception(
                                "All texture layers must have the same mipmap count.");
                        }

                        if (mipWidth != realWidth)
                        {
                            throw love::Exception("Width of image data (slice %d, mipmap level %d) "
                                                  "is incorrect (expected %d, got %d)",
                                                  slice + 1, mipmap + 1, mipWidth, realWidth);
                        }

                        if (mipHeight != realHeight)
                        {
                            throw love::Exception("Height of image data (slice %d, mipmap level "
                                                  "%d) is incorrect (expected %d, got %d)",
                                                  slice + 1, mipmap + 1, mipHeight, realHeight);
                        }

                        if (format != sliceData->GetFormat())
                        {
                            throw love::Exception(
                                "All texture slices and mipmaps must have the same pixel format.");
                        }
                    }

                    mipWidth  = std::max(mipWidth / 2, 1);
                    mipHeight = std::max(mipHeight / 2, 1);

                    if (this->textureType == TEXTURE_VOLUME)
                        mipSlices = std::max(mipSlices / 2, 1);
                }

                return true;
            }

          private:
            TextureType textureType;
            VectorMipmapLayers data;
        };

        static inline int64_t totalGraphicsMemory = 0;
        static inline int textureCount            = 0;

        Texture(const Settings& settings, const Slices* data) :
            textureType(settings.type),
            format(settings.format),
            mipmapMode(settings.mipmaps),
            renderTarget(settings.renderTarget),
            readable(true),
            sRGB(false),
            width(settings.width),
            height(settings.height),
            depth(settings.type == TEXTURE_VOLUME ? settings.layers : 1),
            layers(settings.type == TEXTURE_2D_ARRAY ? settings.layers : 1),
            mipmapCount(1),
            pixelWidth(0),
            pixelHeight(0),
            requestedMSAA(settings.msaa > 1 ? settings.msaa : 0),
            actualSamples(1),
            state {},
            graphicsMemorySize(0),
            slices(settings.type)
        {
            if (data != nullptr && data->GetMipmapCount() > 0 && data->GetSliceCount() > 0)
            {
                this->textureType = data->GetTextureType();

                if (requestedMSAA > 1)
                    throw love::Exception("MSAA textures cannot be created from ImageData.");

                int dataMipmaps = 1;
                if (data->Validate() && data->GetMipmapCount() > 1)
                    dataMipmaps = data->GetMipmapCount();

                const auto* slice = data->Get(0, 0);
                this->format      = slice->GetFormat();

                if (sRGB)
                    this->format = love::GetSRGBPixelFormat(this->format);

                this->pixelWidth  = slice->GetWidth();
                this->pixelHeight = slice->GetHeight();

                if (this->textureType == TEXTURE_2D_ARRAY)
                    this->layers = data->GetSliceCount();
                else if (this->textureType == TEXTURE_VOLUME)
                    this->depth = data->GetSliceCount();

                this->width  = (int)(this->pixelWidth / settings.dpiScale + 0.5);
                this->height = (int)(this->pixelHeight / settings.dpiScale + 0.5);

                if (this->IsCompressed() && dataMipmaps <= 1)
                    this->mipmapMode = MIPMAPS_NONE;
            }
            else
            {
                if (this->IsCompressed())
                    throw love::Exception("Compressed textures must be created with initial data.");

                this->pixelWidth  = (int)((this->width / settings.dpiScale) + 0.5);
                this->pixelHeight = (int)((this->height / settings.dpiScale) + 0.5);
            }

            if (settings.readable.has_value())
                this->readable = settings.readable.value();
            else
            {
                bool isDepthStencilFormat = love::IsPixelFormatDepthStencil(this->format);
                this->readable            = (!this->renderTarget || !isDepthStencilFormat);
            }
        }

        ~Texture()
        {
            --textureCount;
            this->SetGraphicsMemorySize(0);
        }

        MipmapsMode GetMipmapsMode() const
        {
            return this->mipmapMode;
        }

        TextureType GetTextureType() const
        {
            return this->textureType;
        }

        PixelFormat GetPixelFormat() const
        {
            return this->format;
        }

        bool IsCompressed() const
        {
            return love::IsPixelFormatCompressed(this->format);
        }

        int GetWidth(int mipmap = 0) const
        {
            return std::max(this->width >> mipmap, 1);
        }

        bool IsFormatLinear() const
        {
            return !this->sRGB && !love::IsPixelFormatSRGB(this->format);
        }

        int GetHeight(int mipmap = 0) const
        {
            return std::max(this->height >> mipmap, 1);
        }

        int GetDepth(int mipmap = 0) const
        {
            return std::max(this->depth >> mipmap, 1);
        }

        int GetLayerCount() const
        {
            return this->layers;
        }

        int GetMipmapCount() const
        {
            return this->mipmapCount;
        }

        int GetPixelWidth(int mipmap = 0) const
        {
            return std::max(this->pixelWidth >> mipmap, 1);
        }

        int GetPixelHeight(int mipmap = 0) const
        {
            return std::max(this->pixelHeight >> mipmap, 1);
        }

        int GetRequestedMSAA() const
        {
            return this->requestedMSAA;
        }

        int GetDPIScale() const
        {
            return (float)this->pixelHeight / (float)this->height;
        }

        int GetSliceCount(int mipmap) const
        {
            switch (this->textureType)
            {
                case TEXTURE_2D:
                    return 1;
                case TEXTURE_CUBE:
                    return 6;
                case TEXTURE_2D_ARRAY:
                    return this->layers;
                case TEXTURE_VOLUME:
                    return this->GetDepth(mipmap);
                default:
                    break;
            };

            return 1;
        }

        bool IsReadable() const
        {
            return this->readable;
        }

        bool IsRenderTarget() const
        {
            return this->renderTarget;
        }

        bool IsValidSlice(int slice, int mipmap) const
        {
            return slice >= 0 && slice < this->GetSliceCount(mipmap);
        }

        int GetMSAA() const
        {
            return this->actualSamples;
        }

        bool IsComputeWritable() const
        {
            return false;
        }

        void SetSamplerState(const SamplerState& state)
        {
            if (!this->readable)
                return;

            this->state = state;

            if (this->state.mipmapFilter != SamplerState::MIPMAP_FILTER_NONE &&
                this->GetMipmapCount() == 1)
            {
                this->state.mipmapFilter = SamplerState::MIPMAP_FILTER_NONE;
            }
        }

        static int GetTotalMipmapCount(int width, int height)
        {
            return std::log2(std::max(width, height)) + 1;
        }

        static int GetTotalMipmapCount(int width, int height, int depth)
        {
            return std::log2(std::max(std::max(width, height), depth)) + 1;
        }

        const SamplerState& GetSamplerState() const
        {
            return this->state;
        }

        Quad* GetQuad() const
        {
            return this->quad;
        }

        virtual void Draw(Graphics<Console::Which>& graphics,
                          const Matrix4<Console::Which>& transform) = 0;

        virtual void Draw(Graphics<Console::Which>& graphics, Quad* quad,
                          const Matrix4<Console::Which>& transform) = 0;

        // clang-format off
        static constexpr BidirectionalMap textureTypes = {
            "2d",     TEXTURE_2D,
            "volume", TEXTURE_VOLUME,
            "array",  TEXTURE_2D_ARRAY,
            "cube",   TEXTURE_CUBE
        };

        static constexpr BidirectionalMap mipmapModes = {
            "none",   MIPMAPS_NONE,
            "manual", MIPMAPS_MANUAL,
            "auto",   MIPMAPS_AUTO
        };

        static constexpr BidirectionalMap settingsTypes = {
            "width",        SETTING_WIDTH,
            "height",       SETTING_HEIGHT,
            "layers",       SETTING_LAYERS,
            "mipmaps",      SETTING_MIPMAPS,
            "format",       SETTING_FORMAT,
            "linear",       SETTING_LINEAR,
            "type",         SETTING_TYPE,
            "dpiscale",     SETTING_DPI_SCALE,
            "msaa",         SETTING_MSAA,
            "canvas",       SETTING_RENDER_TARGET,
            "computewrite", SETTING_COMPUTE_WRITE,
            "readable",     SETTING_READABLE
        };
        // clang-format on

      protected:
        void CreateTexture()
        {
            this->SetSamplerState(this->state);
        }

        void SetGraphicsMemorySize(int64_t bytes)
        {
            totalGraphicsMemory =
                std::max<int64_t>(totalGraphicsMemory - this->graphicsMemorySize, 0);

            bytes                    = std::max<int64_t>(bytes, 0);
            this->graphicsMemorySize = bytes;

            totalGraphicsMemory += bytes;
        }

        TextureType textureType;
        PixelFormat format;
        MipmapsMode mipmapMode;

        bool renderTarget;
        bool readable;
        bool sRGB;

        int width;
        int height;

        int depth;
        int layers;
        int mipmapCount;

        int pixelWidth;
        int pixelHeight;

        int requestedMSAA;
        int actualSamples;

        SamplerState state;

        StrongReference<Quad> quad;
        int64_t graphicsMemorySize;
        Slices slices;
    };
} // namespace love
