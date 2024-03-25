namespace love
{
// Startup
#define E_UNEXPECTED_ALIGNMENT \
    "Cannot push love object to Lua: unexpected alignment\
    (pointer is %p but alignment should be %d)."

#define E_POINTER_TOO_LARGE "Cannot push love object to Lua: pointer value %p is too large!"

// General
#define E_OUT_OF_MEMORY "Out of memory."
// Filesystem
#define E_PHYSFS_NOT_INITIALIZED     "PHYSFS is not initialized."
#define E_DATA_NOT_WRITTEN           "Data could not be written."
#define E_COULD_NOT_OPEN_FILE        "Could not open file at path {}."
#define E_PHYSFS_COULD_NOT_OPEN_FILE "Could not open file {} {:s}"
#define E_FILE_NOT_OPEN_FOR_WRITING  "File not open for writing."
#define E_INVALID_READ_SIZE          "Invalid read size."
#define E_INVALID_WRITE_SIZE         "Invalid write size."
#define E_NO_FILE_IN_LOVE_DIRS       "\n\tno '{}' in LOVE game directories."
// Audio
#define E_AUDIO_NOT_INITIALIZED "Failed to initialize ndsp"
// Data
#define E_INVALID_SIZE_PARAMETER  "Invalid size parameter (must be greater than 0)."
#define E_INVALID_COUNT_PARAMETER "Invalid count parameter (must be greater than 0)."
#define E_INVALID_OFFSET_AND_SIZE \
    "The given offset and size parameters don't fit within the Data's size."
#define E_DATAVIEW_OFFSET_AND_SIZE \
    "Offset and size of Data View must fit within the original Data's size."
#define E_OFFSET_AND_SIZE_ARGS_FIT_WITHIN_DATA \
    "Offset and size arguments must fit within the given Data's size."
#define E_DATAVIEW_INVALID_SIZE           "DataView size mn ust be greater than 0."
#define E_HASH_FUNCTION_NOT_SUPPORTED     "Hash function not supported by "
#define E_INVALID_COMPRESSION_FORMAT_LZ4  "Invalid format (expecting LZ4)."
#define E_COULD_NOT_LZ4_DECOMPRESS_DATA   "Could not decompress LZ4-compressed data."
#define E_INVALID_COMPRESSION_FORMAT_ZLIB "Invalid format (expecting zlib or gzip)."
#define E_DATA_PACK_OFFSET_FORMAT_PARAMS \
    "The given byte offset and pack format parameters do not fit within the ByteData's size."
#define E_DATA_SIZE_MUST_BE_POSITIVE "Data size must be a positive number."
} // namespace love
