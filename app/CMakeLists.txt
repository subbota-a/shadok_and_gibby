find_package(tomlplusplus REQUIRED)

set(_target shadok_and_gibby)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(${_target} WIN32)
else()
    add_executable(${_target})
endif ()

target_sources(${_target} PRIVATE
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

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    install(TARGETS ${_target} DESTINATION .)
else ()
    install(TARGETS ${_target})
endif ()
