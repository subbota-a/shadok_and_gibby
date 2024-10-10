find_package(tomlplusplus REQUIRED)

set(_target shadok_and_gibby)
add_executable(${_target}
    main.cpp
)

target_link_libraries(${_target}
    PRIVATE
    logic
    domain
    render
    paths
    tomlplusplus::tomlplusplus
)
target_compile_definitions(${_target} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")

install(TARGETS ${_target})
