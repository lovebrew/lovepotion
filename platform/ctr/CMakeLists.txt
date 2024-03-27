dkp_add_asset_target(${PROJECT_NAME}_ctr_romfs romfs)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
)

target_sources(${PROJECT_NAME} PRIVATE
source/boot.cpp
source/driver/EventQueue.cpp
source/modules/system/System.cpp
source/modules/system/wrap_System.cpp
source/modules/timer/Timer.cpp
)
