set(_target paths)
add_library(${_target}
    paths.cpp
    include/paths/paths.h
)

target_include_directories(${_target} PUBLIC include)
