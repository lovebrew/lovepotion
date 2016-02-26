/**
 * @file sfil.h
 * @author Sergi Granell (xerpi)
 * @date 2 April 2015
 * @brief sfillib header
 */

#ifndef SFIL_H
#define SFIL_H

#include <3ds.h>
#include <sf2d.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Loads a PNG image from the SD card
 * @param filename the path of the image to load
 * @param place where to allocate the texture
 * @return a pointer to the newly created texture/image
 */
sf2d_texture *sfil_load_PNG_file(const char *filename, sf2d_place place);

/**
 * @brief Loads a PNG image from a memory buffer
 * @param buffer the pointer of the memory buffer to load the image from
 * @param place where to allocate the texture
 * @return a pointer to the newly created texture/image
 */
sf2d_texture *sfil_load_PNG_buffer(const void *buffer, sf2d_place place);

/**
 * @brief Loads a JPG/JPEG image from the SD card
 * @param filename the path of the image to load
 * @param place where to allocate the texture
 * @return a pointer to the newly created texture/image
 */
sf2d_texture *sfil_load_JPEG_file(const char *filename, sf2d_place place);

/**
 * @brief Loads a JPG/JPEG image from a memory buffer
 * @param buffer the pointer of the memory buffer to load the image from
 * @param buffer_size the size of the memory buffer
 * @param place where to allocate the texture
 * @return a pointer to the newly created texture/image
 */
sf2d_texture *sfil_load_JPEG_buffer(const void *buffer, unsigned long buffer_size, sf2d_place place);

/**
 * @brief Loads a BMP image from the SD card
 * @param filename the path of the image to load
 * @param place where to allocate the texture
 * @return a pointer to the newly created texture/image
 */
sf2d_texture *sfil_load_BMP_file(const char *filename, sf2d_place place);

/**
 * @brief Loads a BMP image from a memory buffer
 * @param buffer the pointer of the memory buffer to load the image from
 * @param place where to allocate the texture
 * @return a pointer to the newly created texture/image
 */
sf2d_texture *sfil_load_BMP_buffer(const void *buffer, sf2d_place place);

#ifdef __cplusplus
}
#endif

#endif
