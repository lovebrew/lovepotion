#pragma once

#include "common/Exception.hpp"
#include "common/Map.hpp"
#include "common/Optional.hpp"
#include "common/int.hpp"
#include "common/math.hpp"
#include "common/pixelformat.hpp"

#include "modules/graphics/Drawable.hpp"
#include "modules/graphics/Quad.hpp"
#include "modules/graphics/Resource.hpp"
#include "modules/graphics/renderstate.hpp"
#include "modules/graphics/samplerstate.hpp"
#include "modules/graphics/vertex.hpp"

#include "modules/image/CompressedImageData.hpp"
#include "modules/image/Image.hpp"
#include "modules/image/ImageData.hpp"

#include <stddef.h>
#include <vector>

namespace love
{
    class GraphicsBase;

    class BufferBase;

    enum TextureType
    {
        TEXTURE_2D,
        TEXTURE_VOLUME,
        TEXTURE_2D_ARRAY,
        TEXTURE_CUBE,
        TEXTURE_MAX_ENUM
    };

    enum PixelFormatUsage
    {
        PIXELFORMATUSAGE_SAMPLE,       // Any sampling in shaders.
        PIXELFORMATUSAGE_LINEAR,       // Linear filtering.
        PIXELFORMATUSAGE_RENDERTARGET, // Usable as a render target.
        PIXELFORMATUSAGE_BLEND,        // Blend support when used as a render target.
        PIXELFORMATUSAGE_MSAA,         // MSAA support when used as a render target.
        PIXELFORMATUSAGE_COMPUTEWRITE, // Writable in compute shaders via imageStore.
        PIXELFORMATUSAGE_MAX_ENUM
    };

    enum PixelFormatUsageFlags
    {
        PIXELFORMATUSAGEFLAGS_NONE         = 0,
        PIXELFORMATUSAGEFLAGS_SAMPLE       = (1 << PIXELFORMATUSAGE_SAMPLE),
        PIXELFORMATUSAGEFLAGS_LINEAR       = (1 << PIXELFORMATUSAGE_LINEAR),
        PIXELFORMATUSAGEFLAGS_RENDERTARGET = (1 << PIXELFORMATUSAGE_RENDERTARGET),
        PIXELFORMATUSAGEFLAGS_BLEND        = (1 << PIXELFORMATUSAGE_BLEND),
        PIXELFORMATUSAGEFLAGS_MSAA         = (1 << PIXELFORMATUSAGE_MSAA),
        PIXELFORMATUSAGEFLAGS_COMPUTEWRITE = (1 << PIXELFORMATUSAGE_COMPUTEWRITE),
    };

    class TextureBase : public Drawable, public Resource
    {
      public:
        static inline Type type = Type("Texture", &Drawable::type);

        static int textureCount;

        enum MipmapsMode
        {
            MIPMAPS_NONE,
            MIPMAPS_AUTO,
            MIPMAPS_MANUAL,
            MIPMAPS_MAX_ENUM
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
            SETTING_VIEW_FORMATS,
            SETTING_READABLE,
            SETTING_DEBUGNAME,
            SETTING_MAX_ENUM
        };

        struct Settings
        {
            int width           = 1;
            int height          = 1;
            int layers          = 1; // depth for 3D textures
            TextureType type    = TEXTURE_2D;
            MipmapsMode mipmaps = MIPMAPS_NONE;
            int mipmapCount     = 0; // only used when > 0.
            PixelFormat format  = PIXELFORMAT_NORMAL;
            bool linear         = false;
            float dpiScale      = 1.0f;
            int msaa            = 1;
            bool renderTarget   = false;
            bool computeWrite   = false;
            std::vector<PixelFormat> viewFormats;
            OptionalBool readable;
            std::string debugName;
        };

        struct ViewSettings
        {
            Optional<PixelFormat> format;
            Optional<TextureType> type;
            OptionalInt mipmapStart;
            OptionalInt mipmapCount;
            OptionalInt layerStart;
            OptionalInt layerCount;
            std::string debugName;
        };

        struct Slices
        {
          public:
            Slices(TextureType textype) : textureType(textype)
            {}

            void clear()
            {
                this->data.clear();
            }

            void set(int slice, int mipmap, ImageDataBase* imageData);

            ImageDataBase* get(int slice, int mipmap) const;

            void add(CompressedImageData* compressedData, int startslice, int startmip, bool addallslices,
                     bool addallmips);

            int getSliceCount(int mip = 0) const;

            int getMipmapCount(int slice = 0) const;

            bool validate() const;

            TextureType getTextureType() const
            {
                return this->textureType;
            }

          private:
            TextureType textureType;

            // For 2D/Cube/2DArray texture types, each element in the data array has
            // an array of mipmap levels. For 3D texture types, each mipmap level
            // has an array of layers.
            std::vector<std::vector<StrongRef<ImageDataBase>>> data;

        }; // Slices

        struct ViewInfo
        {
            TextureBase* texture;
            int startMipmap;
            int startLayer;
        };

        static int64_t totalGraphicsMemory;

        TextureType getTextureType() const
        {
            return this->textureType;
        }

        PixelFormat getPixelFormat() const
        {
            return this->format;
        }

        MipmapsMode getMipmapsMode() const
        {
            return this->mipmapsMode;
        }

        bool isRenderTarget() const
        {
            return this->renderTarget;
        }

        bool isComputeWritable() const
        {
            return this->computeWrite;
        }

        bool isReadable() const
        {
            return this->readable;
        }

        int getMSAA() const
        {
            return this->requestedMSAA;
        }

        const std::vector<PixelFormat>& getViewFormats() const
        {
            return this->viewFormats;
        }

        virtual void updateQuad(Quad* quad)
        {}

        virtual void draw(GraphicsBase* graphics, const Matrix4& matrix) override;

        void draw(GraphicsBase* graphics, Quad* quad, const Matrix4& matrix);

        void drawLayer(GraphicsBase* graphics, int layer, const Matrix4& matrix);

        void drawLayer(GraphicsBase* graphics, int layer, Quad* quad, const Matrix4& matrix);

        bool isCompressed() const
        {
            return love::isPixelFormatCompressed(this->format);
        }

        /* TODO: make the wrapper also check isGammaCorrect */
        bool isFormatLinear() const
        {
            return !isPixelFormatSRGB(this->format);
        }

        const ViewInfo& getRootViewInfo() const
        {
            return this->rootView;
        }

        const ViewInfo& getParentViewInfo() const
        {
            return this->parentView;
        }

        bool isValidSlice(int slice, int mip) const
        {
            return slice >= 0 && slice < this->getSliceCount(mip);
        }

        int getSliceCount(int mip) const;

        void generateMipmaps();

        const std::string& getDebugName() const
        {
            return this->debugName;
        }

        int getWidth(int mipmap = 0) const
        {
            return std::max(this->width >> mipmap, 1);
        }

        int getHeight(int mipmap = 0) const
        {
            return std::max(this->height >> mipmap, 1);
        }

        int getDepth(int mipmap = 0) const
        {
            return std::max(this->depth >> mipmap, 1);
        }

        int getLayerCount() const
        {
            return this->layers;
        }

        int getMipmapCount() const
        {
            return this->mipmapCount;
        }

        int getPixelWidth(int mipmap = 0) const
        {
            return std::max(this->pixelWidth >> mipmap, 1);
        }

        int getPixelHeight(int mipmap = 0) const
        {
            return std::max(this->pixelHeight >> mipmap, 1);
        }

        float getDPIScale() const
        {
            return (float)this->pixelHeight / (float)this->height;
        }

        int getRequestedMSAA() const
        {
            return this->requestedMSAA;
        }

        const SamplerState& getSamplerState() const
        {
            return this->samplerState;
        }

        Quad* getQuad();

        virtual ptrdiff_t getRenderTargetHandle() const = 0;

        virtual ptrdiff_t getSamplerHandle() const = 0;

        /*
         * Sets the handle of the texture.
         * This is ONLY used on 3DS due to memory constraints.
         * The font textures are already on-device!
         */
        virtual void setHandleData(ptrdiff_t data) = 0;

        virtual void setSamplerState(const SamplerState& state) = 0;

        void replacePixels(ImageDataBase* data, int slice, int mipmap, int x, int y, bool reloadMipmaps);

        void replacePixels(const void* data, size_t size, int slice, int mipmap, const Rect& rect,
                           bool reloadMipmaps);

        SamplerState validateSamplerState(SamplerState state) const;

        static int getTotalMipmapCount(int width, int height)
        {
            return (int)std::log2(std::max(width, height)) + 1;
        }

        static int getTotalMipmapCount(int width, int height, int depth)
        {
            return (int)std::log2(std::max(std::max(width, height), depth)) + 1;
        }

        // clang-format off
        STRINGMAP_DECLARE(TextureTypes, TextureType,
            { "2d",        TEXTURE_2D       },
            { "volume",    TEXTURE_VOLUME   },
            { "2darray",   TEXTURE_2D_ARRAY },
            { "cube",      TEXTURE_CUBE     }
        );

        STRINGMAP_DECLARE(MipmapsModes, MipmapsMode,
            { "none",   MIPMAPS_NONE   },
            { "auto",   MIPMAPS_AUTO   },
            { "manual", MIPMAPS_MANUAL }
        );

        STRINGMAP_DECLARE(SettingTypes, SettingType,
            { "width",           SETTING_WIDTH          },
            { "height",          SETTING_HEIGHT         },
            { "layers",          SETTING_LAYERS         },
            { "mipmaps",         SETTING_MIPMAPS        },
            { "mipmapcount",     SETTING_MIPMAP_COUNT   },
            { "format",          SETTING_FORMAT         },
            { "linear",          SETTING_LINEAR         },
            { "type",            SETTING_TYPE           },
            { "dpiscale",        SETTING_DPI_SCALE      },
            { "msaa",            SETTING_MSAA           },
            { "rendertarget",    SETTING_RENDER_TARGET  },
            { "computewrite",    SETTING_COMPUTE_WRITE  },
            { "viewformats",     SETTING_VIEW_FORMATS   },
            { "readable",        SETTING_READABLE       },
            { "debugname",       SETTING_DEBUGNAME      }
        );
        // clang-format on

      protected:
        TextureBase(GraphicsBase* graphics, const Settings& settings, const Slices* slices);

        virtual ~TextureBase();

        void setGraphicsMemorySize(int64_t size);

        void uploadImageData(ImageDataBase* data, int level, int slice, int x, int y);

        virtual void uploadByteData(const void* data, size_t size, int level, int slice,
                                    const Rect& rect) = 0;

        bool supportsGenerateMipmaps(const char*& outReason) const;

        virtual void generateMipmapsInternal() = 0;

        bool validateDimensions(bool throwException) const;

        void validatePixelFormat(GraphicsBase* graphics) const;

        TextureType textureType;
        PixelFormat format;
        bool renderTarget;
        bool computeWrite;
        bool readable;

        std::vector<PixelFormat> viewFormats;
        MipmapsMode mipmapsMode;

        int width;
        int height;

        int depth;
        int layers;
        int mipmapCount;

        int pixelWidth;
        int pixelHeight;

        int requestedMSAA;

        SamplerState samplerState;
        StrongRef<Quad> quad;

        int64_t graphicsMemorySize;
        std::string debugName;

        ViewInfo rootView;
        ViewInfo parentView;

      private:
        void validateViewFormats() const;
    };
} // namespace love
