find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

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
)
target_link_libraries(${_target}
    PUBLIC domain
    PRIVATE SDL2::SDL2 SDL2_image::SDL2_image
)
target_include_directories(${_target} PUBLIC include)
target_compile_definitions(${_target} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")
