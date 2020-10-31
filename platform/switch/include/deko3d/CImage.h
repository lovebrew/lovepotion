/*
** Sample Framework for deko3d Applications
**   CExternalImage.h: Utility class for loading images from the filesystem
*/
#pragma once

#include "deko3d/common.h"
#include "deko3d/CMemPool.h"

#include "libpng16/png.h"

class CImage
{
    dk::Image m_image;
    dk::ImageDescriptor m_descriptor;
    CMemPool::Handle m_mem;

    public:
        CImage() : m_image{},
                   m_descriptor{},
                   m_mem{}
        {}

        ~CImage()
        {
            m_mem.destroy();
        }

        constexpr operator bool() const
        {
            return m_mem;
        }

        constexpr dk::Image & get()
        {
            return m_image;
        }

        constexpr dk::ImageDescriptor const & getDescriptor() const
        {
            return m_descriptor;
        }

        u32 * loadPNG(void * buffer, size_t size, int & width, int & height);

        void loadJPG(void * buffer, size_t size);

        bool load(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device, dk::Queue transferQueue,
                  const char* path, uint32_t width, uint32_t height, DkImageFormat format, uint32_t flags = 0);

        bool loadMemory(CMemPool & imagePool, CMemPool & scratchPool, dk::Device device, dk::Queue transferQueue,
                        void * data, size_t size, uint32_t width, uint32_t height, DkImageFormat format, uint32_t flags = 0);
};
