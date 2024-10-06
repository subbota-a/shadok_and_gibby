find_package(Eigen3 REQUIRED)
find_package(mdspan REQUIRED)

set(_target logic)
add_library(${_target}
    engine.cpp
    include/logic/engine.h
)

target_link_libraries(${_target}
    PUBLIC Eigen3::Eigen domain std::mdspan
)

target_include_directories(${_target} PUBLIC include)
