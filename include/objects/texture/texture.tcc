#pragma once

#include <common/console.hpp>
#include <common/drawable.hpp>
#include <common/exception.hpp>
#include <common/math.hpp>
#include <common/object.hpp>
#include <common/strongreference.hpp>

#include <objects/imagedata/imagedata.tcc>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/driver/renderer/samplerstate.hpp>

#include <stddef.h>

namespace love
{
    using VectorMipmapLayers = std::vector<std::vector<StrongReference<ImageData<Console::Which>>>>;

    template<Console::Platform T = Console::ALL>
    class Texture : public Drawable
    {
      public:
        enum TextureType
        {
            TEXTURE_2D,
            TEXTURE_VOLUME,
            TEXTURE_2D_ARRAY,
            TEXTUERE_CUBE
        };

        static inline Type type = Type("Texture", &Drawable::type);

        static int textureCount;

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

            void Set(int slice, int mipmap, ImageData<Console::Which>* data)
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

            ImageData<Console::Which>* Get(int slice, int mipmap) const
            {
                if (slice < 0 || slice >= this->GetSliceCount(mipmap))
                    return nullptr;

                if (mipmap < 0 || mipmap >= this->GetMipmapCount(slice))
                    return nullptr;

                if (this->textureType == TEXTURE_VOLUME)
                    return this->data[mipmap][slice].Get();

                return this->data[slice][mipmap].Get();
            }

            // void Add(CompressedImageData* data, int startSlice, int startMipmap, bool
            // addAllSlices, bool addAllMipmaps) {}

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

                if (slice < 0 || slice >= (int)data.size())
                    return 0;

                return this->data[slice].size();
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

                if (this->textureType == TEXTUERE_CUBE && sliceCount != 6)
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
                        expectedmips, mipcount);
                }

                if (this->textureType == TEXTUERE_CUBE && width != height)
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
                                                  mip + 1, mipslices, slicecount);
                        }
                    }

                    for (int slice = 0; slice < sliceCount; slice++)
                    {
                        auto* sliceData = this->Get(slice, mipmap);

                        if (!sliceData)
                        {
                            throw love::Exception("Missing image data (slice %d, mipmap level %d)",
                                                  slice + 1, mip + 1);
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
                                                  slice + 1, mip + 1, mipw, realw);
                        }

                        if (mipHeight != realHeight)
                        {
                            throw love::Exception("Height of image data (slice %d, mipmap level "
                                                  "%d) is incorrect (expected %d, got %d)",
                                                  slice + 1, mip + 1, miph, realh);
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

        static int64_t totalGraphicsMemory;

        static int GetTotalMipmapCount(int width, int height, int depth)
        {
            return 0;
        }

        MipmapsMode GetMipmapsMode() const
        {
            return this->mipmapsMode;
        }

        TextureType GetTextureType() const
        {
            return this->textureType;
        }

        PixelFormat GetPixelFormat() const
        {
            return this->pixelFormat;
        }

        bool IsCompressed() const
        {
            return love::IsPixelFormatCompressed(this->format);
        }

        int GetWidth(int mipmap) const
        {
            return std::max(this->width >> mipmap, 1);
        }

        int GetHeight(int mipmap) const
        {
            return std::max(this->height >> mipmap, 1);
        }

        int GetDepth(int mipmap) const
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

        int GetPixelWidth(int mipmap) const
        {
            return std::max(this->pixelWidth >> mipmap, 1);
        }

        int GetPixelHeight(int mipmap) const
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
                case TEXTUERE_CUBE:
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

        bool IsValidSlice(int slice, int mipmap) const
        {
            return slice >= 0 && slice < this->GetSlicecount(mipmap);
        }
        // clang-format off
        static constexpr BidirectionalMap textureTypes = {
            "2d",     TEXTURE_2D,
            "volume", TEXTURE_VOLUME,
            "array",  TEXTURE_2D_ARRAY,
            "cube",   TEXTUERE_CUBE
        };

        static constexpr BidirectionalMap mipmapsMode = {
            "none",   MIPMAPS_NONE,
            "manual", MIPMAPS_MANUAL,
            "auto",   MIPMAPS_AUTO
        };

        static constexpr BidirectionalMap settingsType = {
            "width",        Texture::SETTING_WIDTH,
            "height",       Texture::SETTING_HEIGHT,
            "layers",       Texture::SETTING_LAYERS,
            "mipmaps",      Texture::SETTING_MIPMAPS,
            "format",       Texture::SETTING_FORMAT,
            "linear",       Texture::SETTING_LINEAR,
            "type",         Texture::SETTING_TYPE,
            "dpiscale",     Texture::SETTING_DPI_SCALE,
            "msaa",         Texture::SETTING_MSAA,
            "canvas",       Texture::SETTING_RENDER_TARGET,
            "computewrite", Texture::SETTING_COMPUTE_WRITE,
            "readable",     Texture::SETTING_READABLE
        };
        // clang-format on

      protected:
        void SetGraphicsMemorySize(int64_t size);

        bool ValidateDimensions(bool throwException) const;

        PixelFormat format;
        TextureType textureType;
        MipmapsMode mipmapsMode;

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
        SamplerState state;

        int64_t graphicsMemorySize;
    };
} // namespace love
