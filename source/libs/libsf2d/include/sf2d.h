/**
 * @file sf2d.h
 * @author Sergi Granell (xerpi)
 * @date 22 March 2015
 * @brief sf2dlib header
 */
#ifndef SF2D_H
#define SF2D_H

#include <3ds.h>

#ifdef __cplusplus
extern "C" {
#endif

// Defines

/**
 * @brief Creates a new RGBA8 color
 * @param r the red component of the color to create
 * @param g the green component of the color to create
 * @param b the blue component of the color to create
 * @param a the alpha component of the color to create
 */
#define RGBA8(r, g, b, a) ((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))

#define RGBA8_GET_R(c) (((c) >>  0) & 0xFF)
#define RGBA8_GET_G(c) (((c) >>  8) & 0xFF)
#define RGBA8_GET_B(c) (((c) >> 16) & 0xFF)
#define RGBA8_GET_A(c) (((c) >> 24) & 0xFF)

/**
 * @brief Default size of the GPU commands FIFO buffer
 */
#define SF2D_GPUCMD_DEFAULT_SIZE 0x80000

/**
 * @brief Default size of the temporary memory pool
 */
#define SF2D_TEMPPOOL_DEFAULT_SIZE 0x80000

/**
 * @brief Default depth (Z coordinate) to draw the textures to
 */
#define SF2D_DEFAULT_DEPTH 0.5f

// Enums

/**
 * @brief Represents a texture format
 */

typedef enum {
	TEXFMT_RGBA8  =  0,
	TEXFMT_RGB8   =  1,
	TEXFMT_RGB5A1 =  2,
	TEXFMT_RGB565 =  3,
	TEXFMT_RGBA4  =  4,
	TEXFMT_IA8    =  5,

	TEXFMT_I8     =  7,
	TEXFMT_A8     =  8,
	TEXFMT_IA4    =  9,
	TEXFMT_I4     = 10,
	TEXFMT_A4     = 11,
	TEXFMT_ETC1   = 12,
	TEXFMT_ETC1A4 = 13
} sf2d_texfmt;


/**
 * @brief Data allocated on the RAM or VRAM
 */

typedef enum {
	SF2D_PLACE_RAM,  /**< RAM allocated */
	SF2D_PLACE_VRAM, /**< VRAM allocated */
	SF2D_PLACE_TEMP  /**< Temporary memory pool allocated */
} sf2d_place;

// Structs

/**
 * @brief Represents a two dimensional float vector
 */

typedef struct {
	float u;  /**< First component of the vector */
	float v;  /**< Second component of the vector */
} sf2d_vector_2f;

/**
 * @brief Represents a three dimensional float vector
 */

typedef struct {
	float x;  /**< First component of the vector */
	float y;  /**< Second component of the vector */
	float z;  /**< Third component of the vector */
} sf2d_vector_3f;

/**
 * @brief Represents a vertex containing position (float)
 *        and color (unsigned int)
 */

typedef struct {
	sf2d_vector_3f position;  /**< Position of the vertex */
	u32 color;                /**< Color of the vertex */
} sf2d_vertex_pos_col;

/**
 * @brief Represents a vertex containing position and texture coordinates
 */

typedef struct {
	sf2d_vector_3f position;  /**< Position of the vertex */
	sf2d_vector_2f texcoord;  /**< Texture coordinates of the vertex */
} sf2d_vertex_pos_tex;

/**
 * @brief Represents a texture
 */

typedef struct {
	sf2d_place place;          /**< Where the texture data resides, RAM or VRAM */
	int tiled;                 /**< Whether the tetxure is tiled or not */
	sf2d_texfmt pixel_format;  /**< Pixel format */
	u32 params;                /**< Texture filters and wrapping */
	int width;                 /**< Texture width */
	int height;                /**< Texture height */
	int pow2_w;                /**< Nearest power of 2 >= width */
	int pow2_h;                /**< Nearest power of 2 >= height */
	int data_size;             /**< Size of the raw texture data */
	void *data;                /**< Pointer to the data */
} sf2d_texture;

typedef struct {
	sf2d_texture texture; // "inherit"/extend standard texture
	float projection[4*4];     /**< Orthographic projection matrix for this target */
} sf2d_rendertarget;

// Basic functions

/**
 * @brief Initializates the library
 * @return Whether the initialization has been successful or not
 */
int sf2d_init();

/**
 * @brief Initializates the library (with advanced settings)
 * @param gpucmd_size the size of the GPU FIFO
 * @param temppool_size the size of the temporary pool
 * @return Whether the initialization has been successful or not
 */
int sf2d_init_advanced(int gpucmd_size, int temppool_size);

/**
 * @brief Finishes the library
 * @return Whether the finalization has been successful or not
 */
int sf2d_fini();

/**
 * @brief Enables or disables the 3D
 * @param enable whether to enable or disable the 3D
 */
void sf2d_set_3D(int enable);

/**
 * @brief Starts a frame
 * @param screen target screen
 * @param side target eye (only for top screen)
 */
void sf2d_start_frame(gfxScreen_t screen, gfx3dSide_t side);

/**
 * @brief Starts a frame bound to a rendertarget
 * @param target rendertarget to draw to
 */
void sf2d_start_frame_target(sf2d_rendertarget *target);

/**
 * @brief Ends a frame, should be called on pair with sf2d_start_frame
 */
void sf2d_end_frame();

/**
 * @brief Swaps the framebuffers, should be called once after all the frames have been finished
 */
void sf2d_swapbuffers();

/**
 * @brief Enables or disables the VBlank waiting
 * @param enable whether to enable or disable the VBlank waiting
 */
void sf2d_set_vblank_wait(int enable);

/**
 * @brief Returns the FPS (frames per second)
 * @return the current FPS
 */
float sf2d_get_fps();

/**
 * @brief Allocates memory from a temporary pool. The pool will be emptied after a sf2d_swapbuffers call
 * @param size the number of bytes to allocate
 */
void *sf2d_pool_malloc(u32 size);

/**
 * @brief Allocates aligned memory from a temporary pool. Works as sf2d_pool_malloc
 * @param size the number of bytes to allocate
 * @param alignment the alignment to where allocate the memory
 */
void *sf2d_pool_memalign(u32 size, u32 alignment);

/**
 * @brief Allocates aligned memory for an array from a temporary pool. Works as sf2d_pool_malloc
 * @param nmemb the number of elements to allocate
 * @param size the size (and alignment) of each element to allocate
 * @note Unlike libc's calloc, this function does not initialize to 0,
 *       and returns a pointer aligned to size.
 */
void *sf2d_pool_calloc(u32 nmemb, u32 size);

/**
 * @brief Returns the temporary pool's free space
 * @return the temporary pool's free space
 */
unsigned int sf2d_pool_space_free();

/**
 * @brief Empties the temporary pool
 */
void sf2d_pool_reset();

/**
 * @brief Sets the screen clear color
 * @param color the color
 */
void sf2d_set_clear_color(u32 color);

// Draw functions
/**
 * @brief Draws a line
 * @param x0 x coordinate of the first dot
 * @param y0 y coordinate of the first dot
 * @param x1 x coordinate of the second dot
 * @param y1 y coordinate of the sceond dot
     * @param width thickness of the line
 * @param color the color to draw the line
 */
    void sf2d_draw_line(float x0, float y0, float x1, float y1, float width, u32 color);

/**
 * @brief Draws a rectangle
 * @param x x coordinate of the top left corner of the rectangle
 * @param y y coordinate of the top left corner of the rectangle
 * @param w rectangle width
 * @param h rectangle height
 * @param color the color to draw the rectangle
 */
void sf2d_draw_rectangle(int x, int y, int w, int h, u32 color);

/**
 * @brief Draws a rotated rectangle
 * @param x x coordinate of the top left corner of the rectangle
 * @param y y coordinate of the top left corner of the rectangle
 * @param w rectangle width
 * @param h rectangle height
 * @param color the color to draw the rectangle
 * @param rad rotation (in radians) to draw the rectangle
 */
void sf2d_draw_rectangle_rotate(int x, int y, int w, int h, u32 color, float rad);

/**
 * @brief Draws a filled circle
 * @param x x coordinate of the center of the circle
 * @param y y coordinate of the center of the circle
 * @param radius the radius of the circle
 * @param color the color to draw the circle
 */
void sf2d_draw_fill_circle(int x, int y, int radius, u32 color);

// Texture

/**
 * @brief Creates an empty texture.
 * The returned texture has the data allocated,
 * this means that the raw pixel data can be filled
 * just after the return.
 * @param width the width of the texture
 * @param height the height of the texture
 * @param pixel_format the pixel_format of the texture
 * @param place where to allocate the texture
 * @return a pointer to the newly created texture
 * @note Before drawing the texture, it needs to be tiled
 *       by calling sf2d_texture_tile32.
 *       The default texture params are both min and mag filters
 *       GPU_NEAREST, and both S and T wrappings GPU_CLAMP_TO_BORDER.
 */
sf2d_texture *sf2d_create_texture(int width, int height, sf2d_texfmt pixel_format, sf2d_place place);

/**
 * @brief Creates an empty rendertarget.
 * Functions similarly to sf2d_create_texture.
 * @param width the width of the texture
 * @param height the height of the texture
 * @return a pointer to the newly created rendertarget
 * @note Before drawing the texture, it needs to be tiled
 *       by calling sf2d_texture_tile32.
 *       The default texture params are both min and mag filters
 *       GPU_NEAREST, and both S and T wrappings GPU_CLAMP_TO_BORDER.
 */
sf2d_rendertarget *sf2d_create_rendertarget(int width, int height);

/**
 * @brief Frees a texture
 * @param texture pointer to the texture to freeze
 */
void sf2d_free_texture(sf2d_texture *texture);

/**
 * @brief Frees a rendertarget
 * @param target pointer to the rendertarget to free
 */
void sf2d_free_target(sf2d_rendertarget *target);

/**
 * @brief Clears a rendertarget to the specified color
 * @param target pointer to the rendertarget to clear
 */
void sf2d_clear_target(sf2d_rendertarget *target, u32 color);

/**
 * @brief Fills an already allocated texture from a RGBA8 source
 * @param dst pointer to the destination texture to fill
 * @param rgba8 pointer to the RGBA8 data to fill from
 * @param source_w width (in pixels) of the RGAB8 source
 * @param source_h height (in pixels) of the RGAB8 source
 */
void sf2d_fill_texture_from_RGBA8(sf2d_texture *dst, const void *rgba8, int source_w, int source_h);

/**
 * @brief Creates a texture and fills it from a RGBA8 memory source.
 * The returned texture is already tiled.
 * @param src_buffer pointer to the RGBA8 data to fill from
 * @param src_w width (in pixels) of the RGAB8 source
 * @param src_h height (in pixels) of the RGAB8 source
 * @param pixel_format the pixel_format of the texture to create
 * @param place where to allocate the texture
 * @return a pointer to the newly created, filled, and tiled texture
 */

sf2d_texture *sf2d_create_texture_mem_RGBA8(const void *src_buffer, int src_w, int src_h, sf2d_texfmt pixel_format, sf2d_place place);

/**
 * @brief Binds a texture to a GPU texture unit
 * @param texture the texture to bind
 * @param unit GPU texture unit to bind to
 */
void sf2d_bind_texture(const sf2d_texture *texture, GPU_TEXUNIT unit);

/**
 * @brief Binds a texture to a GPU texture unit with a constant color
 * @param texture the texture to bind
 * @param unit GPU texture unit to bind to
 * @param color the color the bind with the texture
 */
void sf2d_bind_texture_color(const sf2d_texture *texture, GPU_TEXUNIT unit, u32 color);

/**
 * @brief Binds a texture to a GPU texture unit with custom parameters
 * @param texture the texture to bind
 * @param unit GPU texture unit to bind to
 * @param params the parameters the bind with the texture
 */
void sf2d_bind_texture_parameters(const sf2d_texture *texture, GPU_TEXUNIT unit, unsigned int params);

/**
 * @brief Changes the texture params (filters and wrapping)
 * @param texture the texture to change the params
 * @param params the new texture params to use. You can use the
 *        GPU_TEXTURE_[MIN,MAG]_FILTER and GPU_TEXTURE_WRAP_[S,T]
 *        macros as helpers.
 */
void sf2d_texture_set_params(sf2d_texture *texture, u32 params);

/**
 * @brief Returns the texture params
 * @param texture the texture to get the params
 * @return the current texture params of texture
 */
int sf2d_texture_get_params(const sf2d_texture *texture);

/**
 * @brief Draws a texture
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 */
void sf2d_draw_texture(const sf2d_texture *texture, int x, int y);

/**
 * @brief Draws a texture blended with a color
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param color the color to blend with the texture
 */
void sf2d_draw_texture_blend(const sf2d_texture *texture, int x, int y, u32 color);

/**
 * @brief Draws a texture with rotation around a hotspot
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param rad rotation (in radians) to draw the texture
 * @param center_x the x position of the hotspot
 * @param center_y the y position of the hotspot
 */
void sf2d_draw_texture_rotate_hotspot(const sf2d_texture *texture, int x, int y, float rad, float center_x, float center_y);

/**
 * @brief Draws a texture with rotation around a hotspot with a color
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param rad rotation (in radians) to draw the texture
 * @param center_x the x position of the hotspot
 * @param center_y the y position of the hotspot
 * @param color the color to blend with the texture
 */
void sf2d_draw_texture_rotate_hotspot_blend(const sf2d_texture *texture, int x, int y, float rad, float center_x, float center_y, u32 color);


/**
 * @brief Draws a texture with rotation around its center
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param rad rotation (in radians) to draw the texture
 */
void sf2d_draw_texture_rotate(const sf2d_texture *texture, int x, int y, float rad);

/**
 * @brief Draws a texture with rotation around its center with color
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param rad rotation (in radians) to draw the texture
 * @param color the color to blend with the texture
 */
void sf2d_draw_texture_rotate_blend(const sf2d_texture *texture, int x, int y, float rad, u32 color);

/**
 * @brief Draws a scaled texture with rotation around its hotspot
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param rad rotation (in radians) to draw the texture
 * @param x_scale the x scale
 * @param y_scale the y scale
 * @param center_x the x position of the hotspot
 * @param center_y the y position of the hotspot
 */
void sf2d_draw_texture_rotate_scale_hotspot(const sf2d_texture *texture, int x, int y, float rad, float scale_x, float scale_y, float center_x, float center_y);

/**
 * @brief Draws a scaled texture with rotation around its hotspot with color
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param rad rotation (in radians) to draw the texture
 * @param x_scale the x scale
 * @param y_scale the y scale
 * @param center_x the x position of the hotspot
 * @param center_y the y position of the hotspot
 * @param color the color to blend with the texture
 */
void sf2d_draw_texture_rotate_scale_hotspot_blend(const sf2d_texture *texture, int x, int y, float rad, float scale_x, float scale_y, float center_x, float center_y, u32 color); 

/**
 * @brief Draws a part of a texture
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param tex_x the starting point (x coordinate) where to start drawing
 * @param tex_y the starting point (y coordinate) where to start drawing
 * @param tex_w the width to draw from the starting point
 * @param tex_h the height to draw from the starting point
 */
void sf2d_draw_texture_part(const sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h);

/**
 * @brief Draws a part of a texture with color
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param tex_x the starting point (x coordinate) where to start drawing
 * @param tex_y the starting point (y coordinate) where to start drawing
 * @param tex_w the width to draw from the starting point
 * @param tex_h the height to draw from the starting point
 * @param color the color to blend with the texture
 */
void sf2d_draw_texture_part_blend(const sf2d_texture *texture, int x, int y, int tex_x, int tex_y, int tex_w, int tex_h, u32 color);

/**
 * @brief Draws a texture with scaling
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param x_scale the x scale
 * @param y_scale the y scale
 */
void sf2d_draw_texture_scale(const sf2d_texture *texture, int x, int y, float x_scale, float y_scale);

/**
 * @brief Draws a texture with scaling with color
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param x_scale the x scale
 * @param y_scale the y scale
 * @param color the color to blend with the texture
 */
void sf2d_draw_texture_scale_blend(const sf2d_texture *texture, int x, int y, float x_scale, float y_scale, u32 color);

/**
 * @brief Draws a part of a texture, with scaling
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param tex_x the starting point (x coordinate) where to start drawing
 * @param tex_y the starting point (y coordinate) where to start drawing
 * @param tex_w the width to draw from the starting point
 * @param tex_h the height to draw from the starting point
 * @param x_scale the x scale
 * @param y_scale the y scale
 */
void sf2d_draw_texture_part_scale(const sf2d_texture *texture, float x, float y, float tex_x, float tex_y, float tex_w, float tex_h, float x_scale, float y_scale);

/**
 * @brief Draws a part of a texture, with scaling, with color
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param tex_x the starting point (x coordinate) where to start drawing
 * @param tex_y the starting point (y coordinate) where to start drawing
 * @param tex_w the width to draw from the starting point
 * @param tex_h the height to draw from the starting point
 * @param x_scale the x scale
 * @param y_scale the y scale
 * @param color the color to blend with the texture
 */
void sf2d_draw_texture_part_scale_blend(const sf2d_texture *texture, float x, float y, float tex_x, float tex_y, float tex_w, float tex_h, float x_scale, float y_scale, u32 color);

/**
 * @brief Draws a part of a texture, with rotation and scaling
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param rad rotation (in radians) to draw the texture
 * @param tex_x the starting point (x coordinate) where to start drawing
 * @param tex_y the starting point (y coordinate) where to start drawing
 * @param tex_w the width to draw from the starting point
 * @param tex_h the height to draw from the starting point
 * @param x_scale the x scale
 * @param y_scale the y scale
 */
void sf2d_draw_texture_part_rotate_scale(const sf2d_texture *texture, int x, int y, float rad, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale);

/**
 * @brief Draws a part of a texture, with rotation, scaling and color
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param rad rotation (in radians) to draw the texture
 * @param tex_x the starting point (x coordinate) where to start drawing
 * @param tex_y the starting point (y coordinate) where to start drawing
 * @param tex_w the width to draw from the starting point
 * @param tex_h the height to draw from the starting point
 * @param x_scale the x scale
 * @param y_scale the y scale
 * @param color the color to blend with the texture
 */
void sf2d_draw_texture_part_rotate_scale_blend(const sf2d_texture *texture, int x, int y, float rad, int tex_x, int tex_y, int tex_w, int tex_h, float x_scale, float y_scale, u32 color);

/**
 * @brief Draws a texture blended in a certain depth
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param z the depth to draw the texture to
 * @note The z parameter is a value in the [-32768, +32767] range,
 *       where -32768 is the deepest and +32767 the toppest.
 *       By default, the textures are drawn at z = 0.
 *       Keep in mind that this function won't do
 *       Order-independent transparency (OIT), so you should use fully
 *       opaque textures to get good results.
 */
void sf2d_draw_texture_depth(const sf2d_texture *texture, int x, int y, signed short z);

/**
 * @brief Draws a texture blended in a certain depth
 * @param texture the texture to draw
 * @param x the x coordinate to draw the texture to
 * @param y the y coordinate to draw the texture to
 * @param z the depth to draw the texture to
 * @param color the color to blend with the texture
 * @note The z parameter is a value in the [-32768, +32767] range,
 *       where -32768 is the deepest and +32767 the toppest.
 *       By default, the textures are drawn at z = 0.
 *       Keep in mind that this function won't do
 *       Order-independent transparency (OIT), so you should use fully
 *       opaque textures to get good results.
 */
void sf2d_draw_texture_depth_blend(const sf2d_texture *texture, int x, int y, signed short z, u32 color);

/**
 * @brief Draws a texture using custom texture coordinates and parameters
 * @param texture the texture to draw
 * @param left the left coordinate of the texture to start drawing
 * @param top the top coordinate of the texture to start drawing
 * @param width the width to draw from the starting left coordinate
 * @param height the height to draw from the starting top coordinate
 * @param u0 the U texture coordinate of the left vertices
 * @param v0 the V texture coordinate of the top vertices
 * @param u1 the U texture coordinate of the right vertices
 * @param v1 the V texture coordinate of the bottom vertices
 * @param params the parameters to draw the texture with
 */
void sf2d_draw_quad_uv(const sf2d_texture *texture, float left, float top, float right, float bottom,
	float u0, float v0, float u1, float v1, unsigned int params);

/**
 * @brief Changes a pixel of the texture
 * @param texture the texture to change the pixel
 * @param x the x coordinate to change the pixel
 * @param y the y coordinate to change the pixel
 * @param new_color the new color to set to the pixel at (x, y)
 */
void sf2d_set_pixel(sf2d_texture *texture, int x, int y, u32 new_color);

/**
 * @brief Gets a pixel of the texture
 * @param texture the texture to get the pixel
 * @param x the x coordinate to get the pixel
 * @param y the y coordinate to get the pixel
 * @return the pixel at (x, y)
 */
u32 sf2d_get_pixel(sf2d_texture *texture, int x, int y);

/**
 * @brief Tiles a texture
 * @param texture the texture to tile
 */
void sf2d_texture_tile32(sf2d_texture *texture);

/**
 * @brief Sets the scissor test
 * @param mode the test mode (disable, invert or normal)
 * @param x the starting x coordinate of the scissor
 * @param y the starting y coordinate of the scissor
 * @param w the width of the scissor rectangle
 * @param h the height of the scissor rectangle
 * @note This function should be called after sf2d_start_frame.
 *       The scissor will remain active until the sf2d_end_frame call.
 */
void sf2d_set_scissor_test(GPU_SCISSORMODE mode, u32 x, u32 y, u32 w, u32 h);

/**
 * @brief Returns the current screen (latest call to sf2d_start_frame)
 * @note The returned value can be GFX_TOP or GFX_BOTTOM.
 */
gfxScreen_t sf2d_get_current_screen();

/**
 * @brief Returns the current screen side (latest call to sf2d_start_frame)
 * @note The returned value can be GFX_LEFT or GFX_RIGHT.
 */
gfx3dSide_t sf2d_get_current_side();

#ifdef __cplusplus
}
#endif

#endif
