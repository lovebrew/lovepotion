# declare an asset target for the executable's content (optional)
dkp_add_asset_target(${PROJECT_NAME}_cafe_content content)

dkp_track_assets(${PROJECT_NAME}_cafe_content
    FOLDER shaders
    FILES
        color.gsh
        texture.gsh
)

dkp_track_assets(${PROJECT_NAME}_cafe_content
    FOLDER nogame
    FILES
        cartridge.png
        nogame.png
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
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
    source/objects/gamepad.cpp
    source/objects/procontroller.cpp
    ${PROJECT_SOURCE_DIR}/source/objects/truetyperasterizer/truetyperasterizer.cpp
    source/objects/shader_ext.cpp
    source/objects/source_ext.cpp
    source/objects/texture_ext.cpp
    source/objects/wrap_imagedata_ext.cpp
    source/utilities/driver/dsp_ext.cpp
    source/utilities/driver/framebuffer.cpp
    source/utilities/driver/hid_ext.cpp
    source/utilities/driver/renderer_ext.cpp
    source/utilities/haptics/vibration_ext.cpp
    source/utilities/sensor/accelerometer.cpp
    source/utilities/sensor/gyroscope.cpp
    source/utilities/wpad.cpp
)
