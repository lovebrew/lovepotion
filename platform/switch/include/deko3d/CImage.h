/*
** Sample Framework for deko3d Applications
**   CExternalImage.h: Utility class for loading images from the filesystem
*/
#pragma once

#include "deko3d/CMemPool.h"
#include "deko3d/common.h"

#include "common/lmath.h"
#include <memory>

#include "common/pixelformat.h"

class CImage
{
    dk::Image m_image;
    dk::ImageDescriptor m_descriptor;
    CMemPool::Handle m_mem;

  public:
    CImage() : m_image {}, m_descriptor {}, m_mem {}
    {}

    CImage(CImage const&) = delete;

    CImage(CImage&&) = delete;

    ~CImage()
    {
        m_mem.destroy();
    }

    constexpr operator bool() const
    {
        return m_mem;
    }

    constexpr dk::Image& get()
    {
        return m_image;
    }

    constexpr dk::ImageDescriptor const& getDescriptor() const
    {
        return m_descriptor;
    }

    bool load(love::PixelFormat format, bool isSRGB, void* buffer, size_t size, int width,
              int height, bool empty = false);

    bool loadEmptyPixels(CMemPool& imagePool, CMemPool& scratchPool, dk::Device device,
                         dk::Queue queue, uint32_t width, uint32_t height, DkImageFormat format,
                         uint32_t flags = 0);

    bool replacePixels(CMemPool& scratchPool, dk::Device device, const void* data, size_t size,
                       dk::Queue transferQueue, const love::Rect& rect);

    bool loadMemory(CMemPool& imagePool, CMemPool& scratchPool, dk::Device device,
                    dk::Queue transferQueue, const void* data, uint32_t width, uint32_t height,
                    DkImageFormat format, uint32_t flags = 0);

    size_t getFormatSize(DkImageFormat format);

  private:
    std::unique_ptr<u32[]> loadPNG(const void* buffer, const size_t size, int& width, int& height);

    std::unique_ptr<u8[]> loadJPG(const void* buffer, const size_t size, int& width, int& height);
};
