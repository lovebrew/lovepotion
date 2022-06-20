#pragma once

#include "common/exception.h"
#include "common/lmath.h"
#include "common/strongref.h"
#include "objects/drawable/drawable.h"
#include "objects/quad/quad.h"

#include "common/render/samplerstate.h"

#if defined(__SWITCH__)
    #include "deko3d/common.h"
#endif

#include <vector>

namespace love::common
{
    class Texture : public Drawable
    {
      public:
        enum TextureType
        {
            TEXTURE_2D,
            TEXTURE_MAX_ENUM
        };

        static love::Type type;

        static int64_t totalMemory;

        Texture(TextureType texType);
        virtual ~Texture();

        TextureType GetTextureType() const;

        int GetWidth(int mip = 0) const;
        int GetHeight(int mip = 0) const;

        virtual void Draw(Graphics* gfx, const Matrix4& localTransform)                   = 0;
        virtual void Draw(Graphics* gfx, love::Quad* quad, const Matrix4& localTransform) = 0;

        love::Quad* GetQuad() const;

        int GetMipmapCount() const;

        virtual void SetSamplerState(const SamplerState& state) = 0;

        const SamplerState& GetSamplerState() const;

        static int GetTotalMipmapCount(int width, int height);

        static int GetTotalMipmapCount(int width, int height, int depth);

        static int getTotalMipmapCount(int w, int h);
        static int getTotalMipmapCount(int w, int h, int d);

        static bool GetConstant(const char* in, TextureType& out);
        static bool GetConstant(TextureType in, const char*& out);
        static std::vector<const char*> GetConstants(TextureType);

      protected:
        TextureType texType;

        int width;
        int height;
        int mipmapCount;
        int64_t memorySize;

        SamplerState samplerState;
        StrongReference<love::Quad> quad;

        void InitQuad();

        void SetGraphicsMemorySize(int64_t bytes);
    };
} // namespace love::common
