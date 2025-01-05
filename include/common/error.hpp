namespace love
{
// #region Startup
#define E_UNEXPECTED_ALIGNMENT \
    "Cannot push love object to Lua: unexpected alignment\
    (pointer is {} but alignment should be {:d})."
#define E_POINTER_TOO_LARGE "Cannot push love object to Lua: pointer value {} is too large!"
// #endregion

// #region General
#define E_OUT_OF_MEMORY  "Out of memory."
#define E_TITLE_TAKEOVER "Please run LÖVE Potion under Atmosphère title takeover."
// #endregion

// #region Audio
#define E_AUDIO_NOT_INITIALIZED "Failed to initialize ndsp"
#define E_CANNOT_CREATE_QUEUE_SOURCE \
    "Cannot create queueable sources using newSource. Use newQueueableSource instead."
#define E_INVALID_AUDIO_FORMAT   "{:d}-channel Sources with {:d} bits per sample are not supported."
#define E_QUEUE_FORMAT_MISMATCH  "Queued sound data must have same format as sound Source."
#define E_QUEUE_TYPE_MISMATCH    "Only queueable Sources can be queued with sound data."
#define E_QUEUE_LENGTH_MALFORMED "Data length must be a multiple of sample size ({:d} bytes)."
#define E_QUEUE_CANNOT_BE_LOOPED "Queueable Sources can not be looped."
// #endregion

// #region Filesystem
#define E_PHYSFS_NOT_INITIALIZED     "PHYSFS is not initialized."
#define E_DATA_NOT_WRITTEN           "Data could not be written."
#define E_COULD_NOT_OPEN_FILE        "Could not open file at path {:s}."
#define E_PHYSFS_COULD_NOT_OPEN_FILE "Could not open file {:s} ({:s})"
#define E_FILE_NOT_OPEN_FOR_WRITING  "File not open for writing."
#define E_FILE_NOT_OPEN_FOR_READING  "File not open for reading."
#define E_INVALID_READ_SIZE          "Invalid read size."
#define E_INVALID_WRITE_SIZE         "Invalid write size."
#define E_NO_FILE_IN_LOVE_DIRS       "\n\tno '{}' in LOVE game directories."
// #endregion

// #region Data
#define E_INVALID_SIZE_PARAMETER   "Invalid size parameter (must be greater than 0)."
#define E_INVALID_COUNT_PARAMETER  "Invalid count parameter (must be greater than 0)."
#define E_INVALID_OFFSET_AND_SIZE  "The given offset and size parameters don't fit within the Data's size."
#define E_DATAVIEW_OFFSET_AND_SIZE " Offset and size of Data View must fit within the original Data's size."
#define E_OFFSET_AND_SIZE_ARGS_FIT_WITHIN_DATA \
    "Offset and size arguments must fit within the given Data's size."
#define E_DATAVIEW_INVALID_SIZE           "DataView size mn ust be greater than 0."
#define E_HASH_FUNCTION_NOT_SUPPORTED     "Hash function not supported by "
#define E_INVALID_COMPRESSION_FORMAT_LZ4  "Invalid format (expecting LZ4)."
#define E_COULD_NOT_LZ4_DECOMPRESS_DATA   "Could not decompress LZ4-compressed data."
#define E_INVALID_COMPRESSION_FORMAT_ZLIB "Invalid format (expecting zlib or gzip)."
#define E_DATA_PACK_OFFSET_FORMAT_PARAMS \
    "The given byte offset and pack format parameters do not fit within the ByteData's size."
#define E_DATA_SIZE_MUST_BE_POSITIVE       "Data size must be a positive number."
#define E_SOUNDDATA_MISMATCH_CHANNEL_COUNT "Channel count mismatch: {:d} vs {:d}"
// #endregion

// #region Graphics
#define E_BLEND_MIN_MAX_NOT_SUPPORTED "The 'min' and 'max' blend operations are not supported on this system."
// # endregion

// #region Image
#define E_CANNOT_CREATE_TEXTURE "Cannot create texture: {:s} of {:d} is too large for this system."
#define E_CUSTOM_MIPMAP_RANGES_NOT_SUPPORTED                                                                \
    "Custom mipmap ranges for a texture are not supported on this system ({:d} mipmap levels are required " \
    "but only {:d} levels were provided.)"
#define E_COMPRESSED_TEXTURES_MISMATCHED_VIEW                                                           \
    "Compressed textures cannot use different pixel formats for texture views, aside from sRGB versus " \
    "linear variants of the same pixel format."
#define E_COLOR_FORMAT_TEXTURES_MISMATCHED_VIEW \
    "Color-format textures cannot use depth/stencil pixel formats and vice versa, in texture views."
#define E_DEPTH_STENCIL_TEXTURES_MISMATCHED_VIEW                                                       \
    "Using different pixel formats for texture views is not currently supported for depth or stencil " \
    "formats."
#define E_TEXTURE_VIEW_BITS_PER_PIXEL_MISMATCHED \
    "Texture view pixel formats must have the same bits per pixel as the base texture's pixel format."
#define E_TEXTURE_PIXELFORMAT_NOT_SUPPORTED "The {:s} {:s} pixel format is not supported {:s} on this system."
#define E_INVALID_RECT_DIMENSIONS \
    "Invalid rectangle dimensions (x: {:d}, y: {:d}, w: {:d}, h: {:d}) for {:d}x{:d} Texture."
#define E_AUTO_MIPMAPS_NOT_SUPPORTED \
    "Automatic mipmap generation is not supported for textures with the {:s} pixel format."
#define E_CANNOT_GENERATE_MIPMAPS_RT \
    "generateMipmaps cannot be called on this Texture while it's an active render target."
// #endregion

// Thread
#define E_CHANNEL_VARIANT_UNKNOWN "boolean, number, string, love type, or table expected."
#define E_THREAD_VARIANT_UNKNOWN  "boolean, number, string, love type, or flat table expected."

// Window
#define E_WINDOW_CLOSING_RENDERTARGET_ACTIVE \
    "love.window.close cannot be called while a render target is active in love.graphics."
} // namespace love
