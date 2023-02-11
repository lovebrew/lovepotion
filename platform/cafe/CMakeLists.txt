# declare an asset target for the executable's content (optional)
dkp_add_asset_target(${PROJECT_NAME}_cafe_content content)

dkp_track_assets(${PROJECT_NAME}_cafe_content
    FOLDER shaders
    FILES color.gsh texture.gsh
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/common/matrix_ext.cpp
    source/common/screen_ext.cpp
    source/modules/fontmodule_ext.cpp
    source/modules/graphics_ext.cpp
    source/modules/imagemodule_ext.cpp
    source/modules/joystickmodule_ext.cpp
    source/modules/keyboard_ext.cpp
    source/modules/love_ext.cpp
    source/modules/system_ext.cpp
    source/modules/timer_ext.cpp
    source/modules/window_ext.cpp
    source/modules/wrap_fontmodule_ext.cpp
    source/objects/font_ext.cpp
    source/objects/gamepad.cpp
    source/objects/imagedata_ext.cpp
    source/objects/procontroller.cpp
    source/objects/rasterizer_ext.cpp
    source/objects/shader_ext.cpp
    source/objects/source_ext.cpp
    source/objects/texture_ext.cpp
    source/objects/wrap_font_ext.cpp
    source/objects/wrap_imagedata_ext.cpp
    source/utilities/driver/dsp_ext.cpp
    source/utilities/driver/framebuffer.cpp
    source/utilities/driver/hid_ext.cpp
    source/utilities/driver/renderer_ext.cpp
    source/utilities/haptics/vibration_ext.cpp
    source/utilities/sensor/accelerometer.cpp
    source/utilities/sensor/gyroscope.cpp
    source/utilities/threads/thread_ext.cpp
    source/utilities/wpad.cpp
)
