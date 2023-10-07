# declare an asset target for the executable's RomFS (optional)
dkp_add_asset_target(${PROJECT_NAME}_hac_romfs romfs)

# make our shader targets
nx_add_shader_program(transform_vsh "shaders/transform_vsh.glsl" "vert")
nx_add_shader_program(color_fsh     "shaders/color_fsh.glsl"     "frag")
nx_add_shader_program(texture_fsh   "shaders/texture_fsh.glsl"   "frag")
nx_add_shader_program(video_fsh     "shaders/video_fsh.glsl"     "frag")

# install shaders to RomFS
dkp_install_assets (${PROJECT_NAME}_hac_romfs
    DESTINATION "shaders"
    TARGETS
        transform_vsh 
        color_fsh 
        texture_fsh 
        video_fsh
)

dkp_track_assets(${PROJECT_NAME}_hac_romfs
    FOLDER "nogame"
    FILES
        cartridge.png
        nogame.png
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/common/matrix_ext.cpp
    source/common/screen_ext.cpp
    ${PROJECT_SOURCE_DIR}/source/modules/font/fontmodule.cpp
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
    source/objects/joystick_ext.cpp
    ${PROJECT_SOURCE_DIR}/source/objects/truetyperasterizer/truetyperasterizer.cpp
    source/objects/shader_ext.cpp
    source/objects/source_ext.cpp
    source/objects/texture_ext.cpp
    source/objects/wrap_font_ext.cpp
    source/objects/wrap_imagedata_ext.cpp
    source/objects/wrap_joystick_ext.cpp
    source/runtime.cpp
    source/utilities/driver/CIntrusiveTree.cpp
    source/utilities/driver/CMemPool.cpp
    source/utilities/driver/dsp_ext.cpp
    source/utilities/driver/dsp_mem.cpp
    source/utilities/driver/hid_ext.cpp
    source/utilities/driver/renderer_ext.cpp
    source/utilities/haptics/vibration_ext.cpp
    source/utilities/npad.cpp
    source/utilities/sensor/accelerometer.cpp
    source/utilities/sensor/gyroscope.cpp
)
