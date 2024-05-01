#pragma once

#include "common/Exception.hpp"

#include "modules/graphics/Shader.tcc"
#include "modules/graphics/Texture.tcc"
#include "modules/graphics/vertex.hpp"

#if defined(__3DS__)
    #include <3ds.h>

    #define _alloc(size) linearAlloc(size)
    #define _free(ptr)   linearFree(ptr)
#else
    #define _alloc(size) malloc(size)
    #define _free(ptr)   free(ptr)
#endif

namespace love
{
    class StreamBuffer final : public Object
    {
      public:
        struct MapInfo
        {
            uint8_t* data = nullptr;
            size_t size   = 0;

            MapInfo()
            {}

            MapInfo(uint8_t* data, size_t size) : data(data), size(size)
            {}
        };

        StreamBuffer(BufferUsage usage, size_t size) :
            usage(usage),
            data(nullptr),
            bufferSize(size),
            frameGPUReadOffset(0)
        {
            this->data = (uint8_t*)_alloc(size);

            if (this->data == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);

            std::memset(this->data, 0, size);
        }

        virtual ~StreamBuffer()
        {
            _free(this->data);
        }

        MapInfo map()
        {
            return MapInfo(this->data, this->bufferSize);
        }

        size_t unmap()
        {
            return (size_t)this->data;
        }

        size_t getSize() const
        {
            return this->bufferSize;
        }

        size_t getUsableSize() const
        {
            return this->bufferSize - this->frameGPUReadOffset;
        }

        BufferUsage getMode() const
        {
            return this->usage;
        }

        void markUsed(size_t)
        {}

        ptrdiff_t getHandle() const
        {
            return 0;
        }

      private:
        BufferUsage usage;

        uint8_t* data;
        size_t bufferSize;

        size_t frameGPUReadOffset;
    };

    struct DrawCommand
    {
        PrimitiveType primitiveMode           = PRIMITIVE_TRIANGLES;
        ShaderBase::StandardShader shaderType = ShaderBase::STANDARD_DEFAULT;
        TriangleIndexMode indexMode           = TRIANGLEINDEX_NONE;
        CommonFormat format                   = CommonFormat::NONE;

        TextureBase* texture = nullptr;
        int vertexCount      = 0;
    };

    struct DrawIndexedCommand
    {
        PrimitiveType primitiveType = PRIMITIVE_TRIANGLES;
        int indexCount              = 0;
        int instanceCount           = 1;

        IndexDataType indexType = INDEX_UINT16;
        size_t lastPosition     = 0;

        size_t indexBufferOffset = 0;

        TextureBase* texture;
        CullMode cullMode = CULL_NONE;
    };
} // namespace love
