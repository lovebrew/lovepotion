/**
 * @file sftd.h
 * @author Sergi Granell (xerpi)
 * @date 2 April 2015
 * @brief sftdlib header
 */

#ifndef SFTD_H
#define SFTD_H

#include <3ds.h>

#ifdef __cplusplus
extern "C" {
#endif

// Structs

/**
 * @brief Represents a font
 */
typedef struct sftd_font sftd_font;

// Basic functions

/**
 * @brief Initializates the library
 * @return Whether the initialization has been successful or not
 */
int sftd_init();

/**
 * @brief Finishes the library
 * @return Whether the finalization has been successful or not
 */
int sftd_fini();

/**
 * @brief Loads a font from a file
 * @param filename the path to the font
 * @return a pointer to the loaded font (NULL on error)
 */
sftd_font *sftd_load_font_file(const char *filename);

/**
 * @brief Loads a font from memory
 * @param buffer the address of the font
 * @param size the size of the font buffer
 * @return a pointer to the loaded font (NULL on error)
 */
sftd_font *sftd_load_font_mem(const void *buffer, unsigned int size);

/**
 * @brief Frees a font
 * @param font pointer to the font to freeze
 */
void sftd_free_font(sftd_font *font);

// Draw functions

/**
 * @brief Draws text using a font
 * @param font the font to use
 * @param x the x coordinate to draw the text to
 * @param y the y coordinate to draw the text to
 * @param color the color to draw the font
 * @param size the font size
 * @param text a pointer to the text to draw
 */
void sftd_draw_text(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const char *text);

/**
 * @brief Draws formatted text using a font
 * @param font the font to use
 * @param x the x coordinate to draw the text to
 * @param y the y coordinate to draw the text to
 * @param color the color to draw the font
 * @param size the font size
 * @param text a pointer to the text to draw
 * @param ... variable arguments
 */
void sftd_draw_textf(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const char *text, ...);


/**
 * @brief Draws wide text using a font
 * @param font the font to use
 * @param x the x coordinate to draw the text to
 * @param y the y coordinate to draw the text to
 * @param color the color to draw the font
 * @param size the font size
 * @param text a pointer to the wide text to draw
 */
void sftd_draw_wtext(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const wchar_t *text);

/**
 * @brief Draws formatted wide text using a font
 * @param font the font to use
 * @param x the x coordinate to draw the text to
 * @param y the y coordinate to draw the text to
 * @param color the color to draw the font
 * @param size the font size
 * @param text a pointer to the wide text to draw
 * @param ... variable arguments
 */
void sftd_draw_wtextf(sftd_font *font, int x, int y, unsigned int color, unsigned int size, const wchar_t *text, ...);


#ifdef __cplusplus
}
#endif

#endif
