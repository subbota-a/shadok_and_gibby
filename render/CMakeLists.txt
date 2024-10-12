find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

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
)
target_link_libraries(${_target}
    PUBLIC domain paths
    PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf
)
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    target_link_libraries(${_target}
        PRIVATE Shcore
        INTERFACE SDL2::SDL2main
    )
endif()

target_include_directories(${_target} PUBLIC include)
target_compile_definitions(${_target} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")
