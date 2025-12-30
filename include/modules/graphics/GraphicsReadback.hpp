#pragma once

#include "common/Map.hpp"
#include "common/Object.hpp"
#include "common/StrongRef.hpp"
#include "common/int.hpp"
#include "common/math.hpp"
#include "common/pixelformat.hpp"

namespace love
{
    class ImageData;
    class CompressedImageData;
    class ByteData;

    class Buffer;
    class TextureBase;
    class GraphicsBase;

    class GraphicsReadbackBase : public Object
    {
      public:
        enum ReadbackMethod
        {
            READBACK_IMMEDIATE,
            READBACK_ASYNC
        };

        enum Status
        {
            STATUS_WAITING,
            STATUS_COMPLETE,
            STATUS_ERROR
        };

        static Type type;

        GraphicsReadbackBase(GraphicsBase* graphics, ReadbackMethod method, TextureBase* texture, int slice,
                             int mipmap, const Rect& rect, ImageData* destination, int destinationX,
                             int destinationY);

        virtual ~GraphicsReadbackBase();

        virtual void wait()   = 0;
        virtual void update() = 0;

        bool isComplete() const
        {
            return this->status != STATUS_WAITING;
        }

        ReadbackMethod getMethod() const
        {
            return this->method;
        }

        bool hasError() const
        {
            return this->status == STATUS_ERROR;
        }

        ByteData* getBufferData() const;
        ImageData* getImageData() const;

      protected:
        enum DataType
        {
            DATA_BUFFER,
            DATA_TEXTURE
        };

        void* prepareReadbackDestination(size_t size);

        Status readbackBuffer(Buffer* buffer, size_t offset, size_t size);

        DataType dataType;
        ReadbackMethod method;
        Status status = STATUS_WAITING;

        StrongRef<ByteData> bufferData;
        size_t bufferDataOffset = 0;

        StrongRef<ImageData> imageData;
        Rect rect                 = {};
        PixelFormat textureFormat = PIXELFORMAT_UNKNOWN;
        bool isFormatLinear       = false;
        int imageDataX            = 0;
        int imageDataY            = 0;
    };
} // namespace love
