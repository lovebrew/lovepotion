# declare an asset target for the executable's RomFS (optional)
dkp_add_asset_target(${PROJECT_NAME}_hac_romfs romfs)

# make our shader targets
nx_add_shader_program(transform_vsh "platform/hac/shaders/transform_vsh.glsl" "vertex")
nx_add_shader_program(color_fsh     "platform/hac/shaders/color.glsl"         "fragment")
nx_add_shader_program(texture_fsh   "platform/hac/shaders/texture_fsh.glsl"   "fragment")
nx_add_shader_program(video_fsh     "platform/hac/shaders/video_fsh.glsl"     "fragment")

# install shaders to RomFS
dkp_install_assets (${PROJECT_NAME}_hac_romfs
    DESTINATION "shaders"
    TARGETS
        transform_vsh color_fsh texture_fsh video_fsh
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

# find source -type f | grep "\.cpp$" | clip
target_sources(${PROJECT_NAME} PRIVATE
    source/modules/joystickmodule_ext.cpp
    source/modules/love_ext.cpp
    source/modules/system_ext.cpp
    source/modules/timer_ext.cpp
    source/runtime.cpp
    source/utilities/threads/thread_ext.cpp
)
