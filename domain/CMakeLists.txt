find_package(Eigen3 REQUIRED)

set(_target domain)
add_library(${_target} INTERFACE
    include/domain/commands.h
    include/domain/config.h
    include/domain/state.h
    include/domain/units.h
)

target_link_libraries(${_target}
    INTERFACE Eigen3::Eigen
)

target_include_directories(${_target} INTERFACE include)
