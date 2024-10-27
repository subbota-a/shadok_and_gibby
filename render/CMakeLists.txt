find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(magic_enum REQUIRED)

set(_target render)
add_library(${_target}
    engine.cpp
    include/render/engine.h
    include/render/sdl_engine_factory.h
    sdl_engine.cpp
    sdl_engine.h
    sdl_engine_factory.cpp
    surface.cpp
    surface.h
    resources.cpp
    resources.h
    sdl_resources.h
    deleters.h
    deleters.cpp
    sdl_guard.cpp
    sdl_guard.h
    event_controller.cpp
    event_controller.h
)
target_link_libraries(${_target}
    PUBLIC domain paths
    PUBLIC magic_enum::magic_enum
    PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf SDL2_mixer::SDL2_mixer
    INTERFACE SDL2::SDL2main
)
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    target_link_libraries(${_target}
        PRIVATE Shcore
        INTERFACE SDL2::SDL2
    )
endif()

target_include_directories(${_target} PUBLIC include)
target_compile_definitions(${_target} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")
