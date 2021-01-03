/*
** Sample Framework for deko3d Applications
**   CShader.cpp: Utility class for loading shaders from the filesystem
*/
#include "deko3d/CShader.h"

struct DkshHeader
{
    uint32_t magic; // DKSH_MAGIC
    uint32_t header_sz; // sizeof(DkshHeader)
    uint32_t control_sz;
    uint32_t code_sz;
    uint32_t programs_off;
    uint32_t num_programs;
};

bool CShader::loadMemory(CMemPool & pool, const void * buffer, const size_t size)
{
    if (buffer == nullptr || size <= 0)
        return false;

    DkshHeader hdr;

    m_codemem.destroy();

    // Copy the dksh buffer to the struct
    memcpy(&hdr, buffer, sizeof(hdr));

    /*
    ** Allocate the code memory from the pool based on `code_sz`
    ** this must be aligned to DK_SHADER_CODE_ALIGNMENT
    */
    m_codemem = pool.allocate(hdr.code_sz, DK_SHADER_CODE_ALIGNMENT);
    if (!m_codemem)
        goto _failShader;

    /*
    ** Copy the buffer's shader code to the CPU address of the code memory
    ** Make sure to add hdr.control_sz to the buffer for the *correct* offset
    ** As though the buffer were being dealt with like an fread
    */
    memcpy(m_codemem.getCpuAddr(), (char *)buffer + hdr.control_sz, hdr.code_sz);

    /*
    ** Create a shader based on m_codemem's memory block and offset
    ** Set its control to control memory from the dksh and programId
    ** to zero, as that is the entry point of the program
    */
    dk::ShaderMaker{m_codemem.getMemBlock(), m_codemem.getOffset()}
        .setControl(buffer)
        .setProgramId(0)
        .initialize(m_shader);

    return true;

_failShader:
    return false;
}