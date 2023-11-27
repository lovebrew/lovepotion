target_include_directories(${PROJECT_NAME} PRIVATE
    include
    libraries
)

# declare an asset target for the executable's RomFS (optional)
dkp_add_asset_target(${PROJECT_NAME}_ctr_romfs romfs)

ctr_add_graphics_target(cartridge
    IMAGE
    OPTIONS -f rgba8888 -z auto
    INPUTS "assets/nogame/cartridge.png"
)

ctr_add_graphics_target(nogame
    IMAGE
    OPTIONS -f rgba8888 -z auto
    INPUTS "assets/nogame/nogame.png"
)

dkp_install_assets(${PROJECT_NAME}_ctr_romfs
    DESTINATION "nogame"
    TARGETS
        cartridge
        nogame
)

ctr_add_shader_library(main_v_pica
    "assets/shaders/main.v.pica"
)

dkp_install_assets(${PROJECT_NAME}_ctr_romfs
    DESTINATION "shaders"
    TARGETS
        main_v_pica
)

# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
    asm/memcpy.s
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
    source/modules/wrap_graphics_ext.cpp
    source/modules/wrap_system_ext.cpp
    source/objects/imagedata_ext.cpp
    source/objects/joystick_ext.cpp
    source/objects/shader_ext.cpp
    source/objects/source_ext.cpp
    source/objects/texture_ext.cpp
    source/objects/truetyperasterizer_ext.cpp
    source/objects/wrap_imagedata_ext.cpp
    source/runtime.cpp
    source/utilities/driver/dsp_ext.cpp
    source/utilities/driver/hid_ext.cpp
    source/utilities/driver/renderer/framebuffer_ext.cpp
    source/utilities/driver/renderer/renderer_ext.cpp
    source/utilities/formathandler/types/t3xhandler.cpp
    source/utilities/sensor/accelerometer.cpp
    source/utilities/sensor/gyroscope.cpp
)
