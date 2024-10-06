find_package(SDL2 REQUIRED)

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
)

target_link_libraries(${_target}
    PUBLIC domain
    PRIVATE SDL2::SDL2
)

target_include_directories(${_target} PUBLIC include)
