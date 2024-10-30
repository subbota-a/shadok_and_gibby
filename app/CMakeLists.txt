find_package(tomlplusplus REQUIRED)

set(_target shadok_and_gibby)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    add_executable(${_target} WIN32)
    target_sources(${_target} PUBLIC
        windows/resources.rc windows/manifest.manifest
    )
else()
    add_executable(${_target})
endif ()

target_sources(${_target} PRIVATE
    main.cpp
    config.cpp
    config.h
)

target_link_libraries(${_target}
    PRIVATE
    logic
    domain
    ui
    paths
    tomlplusplus::tomlplusplus
)
target_compile_definitions(${_target} PRIVATE PROJECT_NAME="${CMAKE_PROJECT_NAME}")
